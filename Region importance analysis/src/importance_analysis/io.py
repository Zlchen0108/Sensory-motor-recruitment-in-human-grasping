"""Input, output, and parameter-file helpers."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

import pandas as pd


def read_table(path: Path) -> pd.DataFrame:
    """Read a CSV or Excel table."""
    suffix = path.suffix.lower()
    if suffix == ".csv":
        return pd.read_csv(path)
    if suffix in {".xlsx", ".xls"}:
        return pd.read_excel(path)
    raise ValueError(f"Unsupported table format: {suffix}")


def write_table(frame: pd.DataFrame, path: Path) -> None:
    """Write a CSV or Excel table, creating its parent directory."""
    path.parent.mkdir(parents=True, exist_ok=True)
    if path.suffix.lower() == ".csv":
        frame.to_csv(path, index=False)
    elif path.suffix.lower() == ".xlsx":
        frame.to_excel(path, index=False)
    else:
        raise ValueError("Output table must use .csv or .xlsx")


def split_predictors_target(
    frame: pd.DataFrame, target_column: str | None = None
) -> tuple[pd.DataFrame, pd.Series]:
    """Split a table into predictors and a target without silent coercion."""
    if frame.empty:
        raise ValueError("The input table is empty.")
    target = target_column or str(frame.columns[-1])
    if target not in frame.columns:
        raise ValueError(f"Target column not found: {target}")
    predictors = frame.drop(columns=[target])
    if predictors.empty:
        raise ValueError("At least one predictor column is required.")
    return predictors, frame[target]


def read_parameters(path: Path) -> dict[str, dict[str, Any]]:
    """Read model parameters from JSON."""
    with path.open(encoding="utf-8") as stream:
        return json.load(stream)


def write_json(payload: Any, path: Path) -> None:
    """Write deterministic, human-readable JSON."""
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8") as stream:
        json.dump(payload, stream, indent=2, sort_keys=True)
        stream.write("\n")

