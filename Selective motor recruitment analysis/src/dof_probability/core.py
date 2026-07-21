"""Validated calculation functions for joint allocation scores."""

from __future__ import annotations

import json
from importlib.resources import files
from pathlib import Path
from typing import Any

import numpy as np
import pandas as pd


CONSEQUENT_ORDER = [
    "TBFE", "TBBE", "IFFE", "IFME", "IFBE", "TM", "MFFE", "MFBE",
    "MOH", "HMBE", "RFFE", "RFBE", "LFBE", "HMFE", "LFFE",
]


def load_profiles() -> dict[str, dict[str, Any]]:
    """Load the versioned robotic-hand configurations."""
    resource = files("dof_probability").joinpath("profiles.json")
    return json.loads(resource.read_text(encoding="ascii"))


def read_table(path: Path) -> pd.DataFrame:
    """Read a CSV or Excel input table."""
    if path.suffix.lower() == ".csv":
        return pd.read_csv(path)
    if path.suffix.lower() in {".xlsx", ".xls"}:
        return pd.read_excel(path)
    raise ValueError(f"Unsupported input format: {path.suffix}")


def order_and_validate(frame: pd.DataFrame, required_columns: set[str]) -> pd.DataFrame:
    """Validate required fields and apply the documented consequence order."""
    missing = {"consequents", *required_columns} - set(frame.columns)
    if missing:
        raise ValueError(f"Missing required columns: {sorted(missing)}")
    if frame["consequents"].duplicated().any():
        raise ValueError("The consequents column contains duplicate values.")
    indexed = frame.set_index("consequents")
    absent = [value for value in CONSEQUENT_ORDER if value not in indexed.index]
    if absent:
        raise ValueError(f"Missing required consequents: {absent}")
    return indexed.loc[CONSEQUENT_ORDER].reset_index()


def calculate_scores(
    profile_name: str,
    multi_association: pd.DataFrame,
    multi_correlation: pd.DataFrame,
    single_association: pd.DataFrame,
    single_correlation: pd.DataFrame,
) -> pd.DataFrame:
    """Calculate raw and min-max-normalized scores for one hand profile."""
    profiles = load_profiles()
    if profile_name not in profiles:
        raise ValueError(f"Unknown profile: {profile_name}")
    profile = profiles[profile_name]
    terms = profile["score_terms"]
    required_multi = {term["column"] for term in terms if term["scope"] == "multi"}
    required_single = {term["column"] for term in terms if term["scope"] == "single"}
    tables = {
        "multi_association": order_and_validate(multi_association, required_multi),
        "multi_correlation": order_and_validate(multi_correlation, required_multi),
        "single_association": order_and_validate(single_association, required_single),
        "single_correlation": order_and_validate(single_correlation, required_single),
    }
    rows: list[np.ndarray] = []
    for term in terms:
        scope = term["scope"]
        column = term["column"]
        association = tables[f"{scope}_association"][column].to_numpy(dtype=float)
        correlation = tables[f"{scope}_correlation"][column].to_numpy(dtype=float)
        rows.append(float(term["multiplier"]) * (np.abs(association) + np.abs(correlation)))
    score_matrix = np.vstack(rows)
    weights = np.asarray(profile["distance_weights"], dtype=float)
    if weights.shape != score_matrix.shape:
        raise ValueError(
            f"Profile shape mismatch: weights {weights.shape}, scores {score_matrix.shape}"
        )
    raw = np.sum(weights * score_matrix, axis=1)
    span = raw.max() - raw.min()
    normalized = np.zeros_like(raw) if span == 0 else (raw - raw.min()) / span
    return pd.DataFrame(
        {
            "profile": profile_name,
            "joint_id": [f"L{index}" for index in range(1, len(raw) + 1)],
            "raw_score": raw,
            "normalized_score": normalized,
        }
    )


def write_table(frame: pd.DataFrame, path: Path) -> None:
    """Write a result table, creating its parent directory."""
    path.parent.mkdir(parents=True, exist_ok=True)
    if path.suffix.lower() == ".csv":
        frame.to_csv(path, index=False)
    elif path.suffix.lower() == ".xlsx":
        frame.to_excel(path, index=False)
    else:
        raise ValueError("Output must use .csv or .xlsx")

