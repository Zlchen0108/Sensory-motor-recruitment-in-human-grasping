"""Cluster pooled three-dimensional point clouds with seeded K-means."""

from __future__ import annotations

import argparse
import os
from pathlib import Path

os.environ.setdefault("LOKY_MAX_CPU_COUNT", "1")
os.environ.setdefault("OMP_NUM_THREADS", "1")

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from sklearn.cluster import KMeans
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
    parser = argparse.ArgumentParser(description="Cluster pooled three-dimensional point clouds.")
    parser.add_argument("--input-root", type=Path, required=True)
    parser.add_argument("--group", required=True)
    parser.add_argument("--coordinate-set", type=int, required=True)
    parser.add_argument("--file-indices", nargs="+", type=int, default=list(range(1, 53)))
    parser.add_argument(
        "--input-template",
        default="{group}/{file_index}/original_points_{file_index}_{coordinate_set}.xlsx",
    )
    parser.add_argument("--coordinate-columns", nargs=3, default=None)
    parser.add_argument(
        "--axis-labels",
        nargs=3,
        default=["X coordinate (a.u.)", "Y coordinate (a.u.)", "Z coordinate (a.u.)"],
        help="Axis labels including physical units; replace a.u. when calibrated units are known.",
    )
    parser.add_argument("--skip-rows", type=int, default=8)
    parser.add_argument("--clusters", type=int, required=True)
    parser.add_argument("--max-k", type=int, default=15)
    parser.add_argument("--outlier-z-threshold", type=float, default=None)
    parser.add_argument("--seed", type=int, default=20260720)
    parser.add_argument("--output-dir", type=Path, default=Path("clustering_results"))
    return parser.parse_args()


def load_points(args: argparse.Namespace) -> tuple[pd.DataFrame, list[dict[str, object]]]:
    tables: list[pd.DataFrame] = []
    audits: list[dict[str, object]] = []
    for file_index in args.file_indices:
        relative = args.input_template.format(
            group=args.group, file_index=file_index, coordinate_set=args.coordinate_set
        )
        path = args.input_root / relative
        if not path.is_file():
            raise FileNotFoundError(f"Input workbook not found: {path}")
        raw = pd.read_excel(path).iloc[args.skip_rows :].copy()
        points = raw.loc[:, args.coordinate_columns] if args.coordinate_columns else raw.iloc[:, :3]
        points = points.apply(pd.to_numeric, errors="raise")
        points.columns = ["X", "Y", "Z"]
        if not np.isfinite(points.to_numpy()).all():
            raise ValueError(f"Missing or infinite coordinates in {path}.")
        retained = np.ones(len(points), dtype=bool)
        if args.outlier_z_threshold is not None:
            if args.outlier_z_threshold <= 0:
                raise ValueError("--outlier-z-threshold must be positive.")
            standard_deviation = points.std(ddof=0).replace(0, 1)
            z_scores = ((points - points.mean()) / standard_deviation).abs()
            retained = (z_scores <= args.outlier_z_threshold).all(axis=1).to_numpy()
            points = points.loc[retained].copy()
        points["source_file_index"] = file_index
        tables.append(points)
        audits.append(
            {
                "file": str(path.resolve()),
                "sha256": sha256(path),
                "points_before_filtering": int(len(raw)),
                "points_retained": int(retained.sum()),
            }
        )
    return pd.concat(tables, ignore_index=True), audits


def main() -> None:
    args = parse_args()
    points, audits = load_points(args)
    coordinates = points[["X", "Y", "Z"]].to_numpy()
    if not 1 < args.clusters < len(coordinates):
        raise ValueError("--clusters must be greater than 1 and smaller than the number of points.")
    scaler = StandardScaler()
    scaled = scaler.fit_transform(coordinates)
    model = KMeans(n_clusters=args.clusters, random_state=args.seed, n_init=20)
    points["cluster"] = model.fit_predict(scaled)
    centers = pd.DataFrame(scaler.inverse_transform(model.cluster_centers_), columns=["X", "Y", "Z"])
    centers.insert(0, "cluster", np.arange(args.clusters))

    args.output_dir.mkdir(parents=True, exist_ok=True)
    points.to_csv(args.output_dir / "cluster_assignments.csv", index=False)
    centers.to_csv(args.output_dir / "cluster_centers.csv", index=False)
    with pd.ExcelWriter(args.output_dir / "clustering_source_data.xlsx") as writer:
        points.to_excel(writer, sheet_name="Cluster_assignments", index=False)
        centers.to_excel(writer, sheet_name="Cluster_centers", index=False)

    max_k = min(args.max_k, len(coordinates) - 1)
    k_values = np.arange(1, max_k + 1)
    inertia = [
        KMeans(n_clusters=int(k), random_state=args.seed, n_init=20).fit(scaled).inertia_
        for k in k_values
    ]
    pd.DataFrame({"k": k_values, "inertia": inertia}).to_csv(
        args.output_dir / "kmeans_inertia.csv", index=False
    )

    figure = plt.figure(figsize=(3.5, 3.2))
    axis = figure.add_subplot(111, projection="3d")
    axis.scatter(
        points["X"], points["Y"], points["Z"], c=points["cluster"], cmap="tab20", s=3, alpha=0.55
    )
    axis.scatter(centers["X"], centers["Y"], centers["Z"], c="black", marker="x", s=22)
    for row in centers.itertuples(index=False):
        axis.text(row.X, row.Y, row.Z, str(row.cluster), fontsize=6, color="black")
    axis.set_xlabel(args.axis_labels[0])
    axis.set_ylabel(args.axis_labels[1])
    axis.set_zlabel(args.axis_labels[2])
    axis.grid(False)
    for pane_axis in (axis.xaxis, axis.yaxis, axis.zaxis):
        pane_axis.pane.fill = False
        pane_axis.pane.set_edgecolor("#777777")
    figure.tight_layout()
    save_figure(figure, args.output_dir / "cluster_map")
    plt.close(figure)

    figure, axis = plt.subplots(figsize=(3.5, 2.8))
    axis.plot(k_values, inertia, color="#222222", marker="o", markersize=3, linewidth=1)
    axis.set_xlabel("Number of clusters, k")
    axis.set_ylabel("Within-cluster sum of squares")
    axis.set_xticks(k_values)
    figure.tight_layout()
    save_figure(figure, args.output_dir / "kmeans_inertia")
    plt.close(figure)
    write_json(
        {
            "method": "K-means clustering after feature-wise standardization",
            "group": args.group,
            "coordinate_set": args.coordinate_set,
            "clusters": args.clusters,
            "k_selection": "The requested k is not inferred automatically from the elbow plot",
            "random_seed": args.seed,
            "n_init": 20,
            "outlier_filter": args.outlier_z_threshold,
            "coordinate_columns": args.coordinate_columns or "first three columns",
            "axis_labels": args.axis_labels,
            "points_analysed": int(len(points)),
            "inertia_for_selected_k": float(model.inertia_),
            "interpretation": "Exploratory partition; cluster validity requires prespecified external or stability evidence",
            "software_versions": software_versions(
                ["matplotlib", "numpy", "openpyxl", "pandas", "scikit-learn"]
            ),
            "input_files": audits,
        },
        args.output_dir / "analysis_manifest.json",
    )


if __name__ == "__main__":
    main()
