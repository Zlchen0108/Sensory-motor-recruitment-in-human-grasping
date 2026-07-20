"""Compute a cross-dataset Pearson correlation matrix for publication.

For each workbook, correlations are calculated from pairwise-complete numeric
observations. Independent correlation matrices are combined on Fisher's z
scale. The script exports the source-data tables, an analysis manifest, and
editable/high-resolution figure files.
"""

from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
from typing import Sequence

import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns


DEFAULT_GROUPS = ("A", "B", "C", "D", "E", "H", "J")
DEFAULT_CONDITIONS = ("C1", "C2", "C3")
DEFAULT_WORKBOOK = "Merged_Data_Force_For_All_Importance.xlsx"
MIN_PERIODS = 3

mpl.rcParams.update(
    {
        "font.family": "sans-serif",
        "font.sans-serif": ["Arial", "Helvetica", "DejaVu Sans"],
        "font.size": 7,
        "axes.linewidth": 0.8,
        "pdf.fonttype": 42,
        "svg.fonttype": "none",
    }
)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Calculate and export a Fisher-averaged Pearson matrix."
    )
    parser.add_argument("--input-root", type=Path, required=True)
    parser.add_argument("--output-dir", type=Path, default=Path("pearson_results"))
    parser.add_argument("--workbook-name", default=DEFAULT_WORKBOOK)
    parser.add_argument("--groups", nargs="+", default=list(DEFAULT_GROUPS))
    parser.add_argument("--conditions", nargs="+", default=list(DEFAULT_CONDITIONS))
    parser.add_argument(
        "--exclude-columns",
        nargs="*",
        default=[],
        help="Exact column names to exclude before analysis.",
    )
    parser.add_argument(
        "--drop-last-column",
        action="store_true",
        help="Legacy option. Prefer --exclude-columns for an auditable analysis.",
    )
    parser.add_argument("--min-periods", type=int, default=MIN_PERIODS)
    return parser.parse_args()


def file_sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for block in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def load_numeric_data(
    path: Path,
    exclude_columns: Sequence[str],
    drop_last_column: bool,
    min_periods: int,
) -> tuple[pd.DataFrame, dict[str, object]]:
    data = pd.read_excel(path)
    missing_exclusions = sorted(set(exclude_columns) - set(data.columns))
    if missing_exclusions:
        raise ValueError(f"Columns requested for exclusion are absent in {path}: {missing_exclusions}")
    if exclude_columns:
        data = data.drop(columns=list(exclude_columns))
    if drop_last_column:
        if data.shape[1] < 2:
            raise ValueError(f"Cannot drop the final column from {path}: too few columns.")
        data = data.iloc[:, :-1]

    numeric = data.select_dtypes(include="number").replace([np.inf, -np.inf], np.nan)
    nonnumeric = [str(column) for column in data.columns if column not in numeric.columns]
    invalid = [
        str(column)
        for column in numeric.columns
        if numeric[column].dropna().nunique() < 2
    ]
    if invalid:
        raise ValueError(f"All-missing or constant numeric columns in {path}: {invalid}")
    insufficient = [
        str(column) for column in numeric.columns if numeric[column].notna().sum() < min_periods
    ]
    if insufficient:
        raise ValueError(
            f"Numeric columns with fewer than {min_periods} observations in {path}: {insufficient}"
        )
    if numeric.shape[1] < 2:
        raise ValueError(f"At least two variable numeric columns are required in {path}.")

    audit = {
        "file": str(path.resolve()),
        "sha256": file_sha256(path),
        "rows": int(len(data)),
        "numeric_columns": int(numeric.shape[1]),
        "nonnumeric_columns_ignored": nonnumeric,
        "missing_values_by_column": {
            str(key): int(value) for key, value in numeric.isna().sum().items()
        },
    }
    return numeric, audit


def fisher_mean(matrices: Sequence[pd.DataFrame]) -> tuple[pd.DataFrame, pd.DataFrame]:
    values = np.stack([matrix.to_numpy(dtype=float) for matrix in matrices])
    valid = np.isfinite(values)
    counts = valid.sum(axis=0)
    clipped = np.clip(values, -0.999999, 0.999999)
    z_values = np.where(valid, np.arctanh(clipped), np.nan)
    with np.errstate(invalid="ignore"):
        pooled = np.tanh(np.nanmean(z_values, axis=0))
    np.fill_diagonal(pooled, 1.0)

    labels = matrices[0].columns
    pooled_frame = pd.DataFrame(pooled, index=labels, columns=labels)
    count_frame = pd.DataFrame(counts, index=labels, columns=labels)
    return pooled_frame, count_frame


def plot_heatmap(matrix: pd.DataFrame, output_stem: Path) -> None:
    variable_count = len(matrix)
    size_inches = min(max(3.5, 0.34 * variable_count), 7.2)
    figure, axis = plt.subplots(figsize=(size_inches, size_inches))
    mask = np.triu(np.ones_like(matrix, dtype=bool), k=1)
    sns.heatmap(
        matrix,
        mask=mask,
        vmin=-1,
        vmax=1,
        center=0,
        cmap="vlag",
        square=True,
        annot=variable_count <= 20,
        fmt=".2f",
        annot_kws={"size": 5},
        linewidths=0.25,
        linecolor="white",
        cbar_kws={"label": "Pearson correlation coefficient (r)", "shrink": 0.72},
        ax=axis,
    )
    axis.set_xlabel("")
    axis.set_ylabel("")
    axis.tick_params(axis="x", rotation=45, length=2, width=0.6)
    axis.tick_params(axis="y", rotation=0, length=2, width=0.6)
    figure.tight_layout()
    figure.savefig(output_stem.with_suffix(".svg"), bbox_inches="tight")
    figure.savefig(output_stem.with_suffix(".pdf"), bbox_inches="tight")
    figure.savefig(output_stem.with_suffix(".tiff"), dpi=600, bbox_inches="tight")
    plt.close(figure)


def run_analysis(args: argparse.Namespace) -> None:
    if args.min_periods < 3:
        raise ValueError("--min-periods must be at least 3.")
    args.output_dir.mkdir(parents=True, exist_ok=True)
    matrices: list[pd.DataFrame] = []
    dataset_names: list[str] = []
    audits: list[dict[str, object]] = []
    reference_columns: list[str] | None = None

    for group in args.groups:
        for condition in args.conditions:
            name = f"{group}_{condition}"
            path = args.input_root / group / condition / args.workbook_name
            if not path.is_file():
                raise FileNotFoundError(f"Input workbook not found: {path}")
            data, audit = load_numeric_data(
                path, args.exclude_columns, args.drop_last_column, args.min_periods
            )
            columns = [str(column) for column in data.columns]
            data.columns = columns
            if reference_columns is None:
                reference_columns = columns
            elif columns != reference_columns:
                raise ValueError(
                    f"Column mismatch in {path}. Expected {reference_columns}; observed {columns}."
                )
            matrices.append(data.corr(method="pearson", min_periods=args.min_periods))
            dataset_names.append(name)
            audits.append({"dataset": name, **audit})

    pooled, counts = fisher_mean(matrices)
    with pd.ExcelWriter(args.output_dir / "pearson_source_data.xlsx") as writer:
        pooled.to_excel(writer, sheet_name="Fisher_mean_Pearson")
        counts.to_excel(writer, sheet_name="Contributing_datasets")
        for name, matrix in zip(dataset_names, matrices):
            matrix.to_excel(writer, sheet_name=name[:31])
    pooled.to_csv(args.output_dir / "pearson_matrix.csv", encoding="utf-8")

    manifest = {
        "analysis": "Pearson correlation with pairwise-complete observations",
        "aggregation": "Unweighted arithmetic mean on Fisher's z scale",
        "number_of_datasets": len(matrices),
        "minimum_pairwise_observations": args.min_periods,
        "groups": args.groups,
        "conditions": args.conditions,
        "excluded_columns": args.exclude_columns,
        "legacy_final_column_excluded": args.drop_last_column,
        "datasets": audits,
    }
    (args.output_dir / "analysis_manifest.json").write_text(
        json.dumps(manifest, indent=2), encoding="utf-8"
    )
    plot_heatmap(pooled, args.output_dir / "pearson_heatmap")
    print(f"Analysed {len(matrices)} datasets.")
    print(f"Results saved to: {args.output_dir.resolve()}")


def main() -> None:
    run_analysis(parse_args())


if __name__ == "__main__":
    main()
