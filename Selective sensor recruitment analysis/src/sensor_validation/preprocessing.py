"""Data preparation and predefined tactile-sensor masks."""

from __future__ import annotations

import re
from pathlib import Path

import pandas as pd

from .io import read_table


SENSOR_MASKS = {
    "reduced_5": {28, 29, 37, 38},
    "reduced_4": {1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 23, 24, 25, 26, 28, 29, 33, 34, 35, 36, 37, 38, 39, 41, 42, 43},
    "reduced_3": {1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 18, 23, 24, 25, 26, 28, 29, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43},
    "reduced_2": {1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 18, 23, 24, 25, 26, 28, 29, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 46, 47},
}


def natural_key(path: Path) -> list[object]:
    return [int(part) if part.isdigit() else part.lower() for part in re.split(r"([0-9]+)", path.stem)]


def prepare_recordings(
    paths: list[Path], window_size: int, drop_leading_columns: int, label_column: str
) -> pd.DataFrame:
    """Create one labelled analysis table from individual recordings."""
    if window_size <= 0:
        raise ValueError("window_size must be positive")
    prepared: list[pd.DataFrame] = []
    for sequence, path in enumerate(sorted(paths, key=natural_key), start=1):
        frame = read_table(path)
        if len(frame) < window_size:
            raise ValueError(f"Recording has fewer than {window_size} rows: {path}")
        midpoint = len(frame) // 2
        start = midpoint - window_size // 2
        start = min(max(start, 0), len(frame) - window_size)
        subset = frame.iloc[start : start + window_size, drop_leading_columns:].copy()
        numeric = subset.select_dtypes(include="number").columns
        subset[numeric] = subset[numeric].clip(lower=0).fillna(0)
        subset[label_column] = int(path.stem) if path.stem.isdigit() else sequence
        prepared.append(subset)
    if not prepared:
        raise ValueError("No recording files matched the input pattern.")
    return pd.concat(prepared, ignore_index=True)


def apply_sensor_mask(frame: pd.DataFrame, mask_name: str) -> pd.DataFrame:
    """Set selected English-named sensor channels to zero."""
    if mask_name not in SENSOR_MASKS:
        raise ValueError(f"Unknown sensor mask: {mask_name}")
    output = frame.copy()
    expected = {f"channel_{index}" for index in SENSOR_MASKS[mask_name]}
    missing = expected - set(output.columns)
    if missing:
        raise ValueError(f"Missing sensor columns required by {mask_name}: {sorted(missing)}")
    output[list(expected)] = 0
    return output

