"""Perform a reproducible two-sided Mantel permutation test.

Both inputs must be symmetric distance matrices with identical sample labels.
Only the strict upper triangles are correlated. The same random permutation is
applied to the rows and columns of the second matrix at every iteration.
"""

from __future__ import annotations

import argparse
import json
from pathlib import Path

import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


mpl.rcParams.update(
    {
        "font.family": "sans-serif",
        "font.sans-serif": ["Arial", "Helvetica", "DejaVu Sans"],
        "font.size": 7,
        "axes.spines.right": False,
        "axes.spines.top": False,
        "axes.linewidth": 0.8,
        "pdf.fonttype": 42,
        "svg.fonttype": "none",
    }
)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Run a two-sided Mantel permutation test.")
    parser.add_argument("matrix_a", type=Path)
    parser.add_argument("matrix_b", type=Path)
    parser.add_argument("--output-dir", type=Path, default=Path("mantel_results"))
    parser.add_argument("--permutations", type=int, default=9999)
    parser.add_argument("--seed", type=int, default=20260720)
    parser.add_argument("--label-a", default="Distance matrix A")
    parser.add_argument("--label-b", default="Distance matrix B")
    return parser.parse_args()


def read_matrix(path: Path) -> pd.DataFrame:
    suffix = path.suffix.lower()
    if suffix in {".xlsx", ".xls"}:
        matrix = pd.read_excel(path, index_col=0)
    elif suffix in {".csv", ".txt", ".tsv"}:
        separator = "\t" if suffix in {".txt", ".tsv"} else ","
        matrix = pd.read_csv(path, index_col=0, sep=separator)
    else:
        raise ValueError(f"Unsupported matrix format: {path.suffix}")
    matrix.index = matrix.index.map(str)
    matrix.columns = matrix.columns.map(str)
    return matrix.apply(pd.to_numeric, errors="raise")


def validate_matrices(matrix_a: pd.DataFrame, matrix_b: pd.DataFrame) -> pd.DataFrame:
    if matrix_a.shape[0] != matrix_a.shape[1] or matrix_b.shape[0] != matrix_b.shape[1]:
        raise ValueError("Both inputs must be square matrices.")
    if matrix_a.index.tolist() != matrix_a.columns.tolist():
        raise ValueError("Matrix A row and column labels must match in the same order.")
    if set(matrix_a.index) != set(matrix_b.index) or set(matrix_b.index) != set(matrix_b.columns):
        raise ValueError("Both matrices must contain identical row and column labels.")
    matrix_b = matrix_b.loc[matrix_a.index, matrix_a.index]
    for name, matrix in (("A", matrix_a), ("B", matrix_b)):
        values = matrix.to_numpy(dtype=float)
        if not np.isfinite(values).all():
            raise ValueError(f"Matrix {name} contains missing or infinite values.")
        if not np.allclose(values, values.T, rtol=1e-7, atol=1e-10):
            raise ValueError(f"Matrix {name} is not symmetric.")
        if not np.allclose(np.diag(values), 0.0, atol=1e-10):
            raise ValueError(f"Matrix {name} must have a zero diagonal.")
    if len(matrix_a) < 3:
        raise ValueError("At least three matched samples are required.")
    return matrix_b


def upper_triangle(values: np.ndarray) -> np.ndarray:
    indices = np.triu_indices_from(values, k=1)
    return values[indices]


def pearson_r(x: np.ndarray, y: np.ndarray) -> float:
    if np.std(x) == 0 or np.std(y) == 0:
        raise ValueError("A distance vector is constant; Pearson correlation is undefined.")
    return float(np.corrcoef(x, y)[0, 1])


def mantel_test(
    matrix_a: np.ndarray, matrix_b: np.ndarray, permutations: int, seed: int
) -> tuple[float, float, np.ndarray]:
    if permutations < 999:
        raise ValueError("Use at least 999 permutations for stable inference.")
    vector_a = upper_triangle(matrix_a)
    observed = pearson_r(vector_a, upper_triangle(matrix_b))
    rng = np.random.default_rng(seed)
    null_distribution = np.empty(permutations, dtype=float)
    for index in range(permutations):
        order = rng.permutation(matrix_b.shape[0])
        permuted = matrix_b[np.ix_(order, order)]
        null_distribution[index] = pearson_r(vector_a, upper_triangle(permuted))
    p_value = (np.count_nonzero(np.abs(null_distribution) >= abs(observed)) + 1) / (
        permutations + 1
    )
    return observed, float(p_value), null_distribution


def plot_relationship(
    x: np.ndarray,
    y: np.ndarray,
    observed: float,
    p_value: float,
    label_a: str,
    label_b: str,
    output_stem: Path,
) -> None:
    figure, axis = plt.subplots(figsize=(3.5, 3.0))
    axis.scatter(x, y, s=12, color="#3B6FB6", alpha=0.65, edgecolors="none")
    slope, intercept = np.polyfit(x, y, deg=1)
    line_x = np.linspace(x.min(), x.max(), 100)
    axis.plot(line_x, slope * line_x + intercept, color="#222222", linewidth=1.0)
    p_text = f"P = {p_value:.4f}" if p_value >= 0.0001 else "P < 0.0001"
    axis.text(
        0.04,
        0.96,
        f"Mantel r = {observed:.3f}\n{p_text}",
        transform=axis.transAxes,
        va="top",
    )
    axis.set_xlabel(label_a)
    axis.set_ylabel(label_b)
    figure.tight_layout()
    figure.savefig(output_stem.with_suffix(".svg"), bbox_inches="tight")
    figure.savefig(output_stem.with_suffix(".pdf"), bbox_inches="tight")
    figure.savefig(output_stem.with_suffix(".tiff"), dpi=600, bbox_inches="tight")
    plt.close(figure)


def main() -> None:
    args = parse_args()
    matrix_a = read_matrix(args.matrix_a)
    matrix_b = validate_matrices(matrix_a, read_matrix(args.matrix_b))
    values_a = matrix_a.to_numpy(dtype=float)
    values_b = matrix_b.to_numpy(dtype=float)
    observed, p_value, null_distribution = mantel_test(
        values_a, values_b, args.permutations, args.seed
    )

    args.output_dir.mkdir(parents=True, exist_ok=True)
    result = {
        "test": "Two-sided Pearson Mantel permutation test",
        "mantel_r": observed,
        "permutation_p_value": p_value,
        "permutations": args.permutations,
        "random_seed": args.seed,
        "matched_samples": len(matrix_a),
        "distance_pairs": len(upper_triangle(values_a)),
        "matrix_a": str(args.matrix_a.resolve()),
        "matrix_b": str(args.matrix_b.resolve()),
    }
    (args.output_dir / "mantel_result.json").write_text(
        json.dumps(result, indent=2), encoding="utf-8"
    )
    pd.DataFrame({"permuted_r": null_distribution}).to_csv(
        args.output_dir / "mantel_null_distribution.csv", index=False
    )
    pd.DataFrame(
        {
            args.label_a: upper_triangle(values_a),
            args.label_b: upper_triangle(values_b),
        }
    ).to_csv(args.output_dir / "mantel_plot_source_data.csv", index=False)
    plot_relationship(
        upper_triangle(values_a),
        upper_triangle(values_b),
        observed,
        p_value,
        args.label_a,
        args.label_b,
        args.output_dir / "mantel_relationship",
    )
    print(json.dumps(result, indent=2))


if __name__ == "__main__":
    main()
