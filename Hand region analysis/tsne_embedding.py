"""Generate a seeded joint t-SNE embedding for exploratory visualization."""

from __future__ import annotations

import argparse
import os
from pathlib import Path

os.environ.setdefault("LOKY_MAX_CPU_COUNT", "1")
os.environ.setdefault("OMP_NUM_THREADS", "1")

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from sklearn.manifold import TSNE
from sklearn.preprocessing import StandardScaler

from publication_utils import (
    configure_matplotlib,
    save_figure,
    sha256,
    software_versions,
    write_json,
)


configure_matplotlib()


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Generate a joint t-SNE embedding.")
    parser.add_argument("inputs", nargs="+", type=Path)
    parser.add_argument(
        "--source-labels",
        nargs="+",
        default=None,
        help="Optional publication labels matching the input-file order.",
    )
    parser.add_argument("--feature-columns", nargs="+", default=["X", "Y", "Z"])
    parser.add_argument("--metadata-columns", nargs="*", default=["file_number"])
    parser.add_argument("--components", type=int, choices=[1, 2], default=2)
    parser.add_argument("--perplexity", type=float, default=30.0)
    parser.add_argument("--max-iter", type=int, default=1000)
    parser.add_argument("--seed", type=int, default=20260720)
    parser.add_argument("--output-dir", type=Path, default=Path("tsne_results"))
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    tables: list[pd.DataFrame] = []
    audits: list[dict[str, object]] = []
    if args.source_labels is not None and len(args.source_labels) != len(args.inputs):
        raise ValueError("--source-labels must contain exactly one label per input file.")
    source_labels = args.source_labels or [f"Dataset {index}" for index in range(1, len(args.inputs) + 1)]
    for path, source_label in zip(args.inputs, source_labels):
        if not path.is_file():
            raise FileNotFoundError(f"Input workbook not found: {path}")
        data = pd.read_excel(path)
        required = set(args.feature_columns + args.metadata_columns)
        if not required.issubset(data.columns):
            raise ValueError(f"Missing columns in {path}: {sorted(required - set(data.columns))}")
        selected = data[args.feature_columns + args.metadata_columns].copy()
        selected.insert(0, "source_file", path.name)
        selected.insert(1, "source_label", source_label)
        tables.append(selected)
        audits.append({"file": str(path.resolve()), "sha256": sha256(path), "rows": len(data)})
    combined = pd.concat(tables, ignore_index=True)
    features = combined[args.feature_columns].apply(pd.to_numeric, errors="raise").to_numpy()
    if not np.isfinite(features).all():
        raise ValueError("Feature data contain missing or infinite values.")
    if not 0 < args.perplexity < len(features):
        raise ValueError("--perplexity must be positive and smaller than the total sample size.")
    if args.max_iter < 250:
        raise ValueError("--max-iter must be at least 250.")
    standardized = StandardScaler().fit_transform(features)
    embedding = TSNE(
        n_components=args.components,
        perplexity=args.perplexity,
        init="pca",
        learning_rate="auto",
        random_state=args.seed,
        max_iter=args.max_iter,
    ).fit_transform(standardized)
    output = combined[["source_file", "source_label", *args.metadata_columns]].copy()
    for index in range(args.components):
        output[f"TSNE{index + 1}"] = embedding[:, index]
    args.output_dir.mkdir(parents=True, exist_ok=True)
    output.to_csv(args.output_dir / "tsne_embedding.csv", index=False)
    output.to_excel(args.output_dir / "tsne_embedding.xlsx", index=False)

    if args.components == 2:
        figure, axis = plt.subplots(figsize=(3.5, 3.0))
        categories, codes = np.unique(output["source_label"], return_inverse=True)
        points = axis.scatter(output["TSNE1"], output["TSNE2"], c=codes, cmap="tab20", s=4, alpha=0.6)
        axis.set_xlabel("t-SNE 1 (a.u.)")
        axis.set_ylabel("t-SNE 2 (a.u.)")
        if len(categories) <= 12:
            handles = points.legend_elements()[0]
            axis.legend(handles, categories, title="Source", fontsize=5, title_fontsize=6)
        figure.tight_layout()
        save_figure(figure, args.output_dir / "tsne_embedding")
        plt.close(figure)
    write_json(
        {
            "method": "Joint t-SNE embedding after feature-wise standardization",
            "interpretation": "Exploratory visualization only; distances and apparent clusters are not inferential evidence",
            "components": args.components,
            "perplexity": args.perplexity,
            "random_seed": args.seed,
            "maximum_iterations": args.max_iter,
            "samples_embedded": int(len(output)),
            "feature_columns": args.feature_columns,
            "source_labels": source_labels,
            "software_versions": software_versions(
                ["matplotlib", "numpy", "openpyxl", "pandas", "scikit-learn"]
            ),
            "input_files": audits,
        },
        args.output_dir / "analysis_manifest.json",
    )


if __name__ == "__main__":
    main()
