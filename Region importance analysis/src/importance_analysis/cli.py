"""Command-line interface for the reproducible analysis workflow."""

from __future__ import annotations

import argparse
import glob
from pathlib import Path

from .analysis import (
    cluster_point_clouds,
    embed_cluster,
    feature_importance,
    optimize_regressors,
    shap_importance,
    validate_classifiers,
)
from .io import read_parameters, read_table, split_predictors_target, write_json, write_table


def add_common_table_arguments(parser: argparse.ArgumentParser) -> None:
    parser.add_argument("--input", type=Path, required=True, help="Input CSV or Excel table.")
    parser.add_argument("--target-column", help="Target column; defaults to the final column.")
    parser.add_argument("--random-state", type=int, default=1412)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(prog="importance-analysis")
    commands = parser.add_subparsers(dest="command", required=True)

    cluster = commands.add_parser("cluster", help="Cluster three-dimensional point clouds.")
    cluster.add_argument("--input-glob", required=True)
    cluster.add_argument("--output-dir", type=Path, required=True)
    cluster.add_argument("--n-clusters", type=int, default=15)
    cluster.add_argument("--skip-rows", type=int, default=8)
    cluster.add_argument("--random-state", type=int, default=1412)

    embed = commands.add_parser("embed", help="Compute normalized one-dimensional t-SNE embeddings.")
    embed.add_argument("--input-glob", required=True)
    embed.add_argument("--output-dir", type=Path, required=True)
    embed.add_argument("--random-state", type=int, default=1412)

    optimize = commands.add_parser("optimize", help="Optimize tree regressors.")
    add_common_table_arguments(optimize)
    optimize.add_argument("--output", type=Path, required=True)
    optimize.add_argument("--n-trials", type=int, default=100)

    importance = commands.add_parser("importance", help="Calculate native feature importances.")
    add_common_table_arguments(importance)
    importance.add_argument("--parameters", type=Path, required=True)
    importance.add_argument("--output", type=Path, required=True)

    validate = commands.add_parser("validate", help="Validate tree classifiers on held-out data.")
    add_common_table_arguments(validate)
    validate.add_argument("--parameters", type=Path, required=True)
    validate.add_argument("--output", type=Path, required=True)
    validate.add_argument("--test-size", type=float, default=0.2)

    shap_parser = commands.add_parser("shap", help="Calculate gradient-boosting SHAP importance.")
    add_common_table_arguments(shap_parser)
    shap_parser.add_argument("--parameters", type=Path, required=True)
    shap_parser.add_argument("--output", type=Path, required=True)
    return parser


def load_xy(args: argparse.Namespace):
    return split_predictors_target(read_table(args.input), args.target_column)


def main() -> None:
    args = build_parser().parse_args()
    if args.command == "cluster":
        files = [Path(path) for path in sorted(glob.glob(args.input_glob))]
        points, centers = cluster_point_clouds(
            files, args.n_clusters, args.skip_rows, args.random_state
        )
        args.output_dir.mkdir(parents=True, exist_ok=True)
        write_table(points, args.output_dir / "clustered_points.csv")
        write_table(centers, args.output_dir / "cluster_centers.csv")
        for cluster_id, group in points.groupby("cluster"):
            write_table(group, args.output_dir / f"cluster_{cluster_id}.csv")
    elif args.command == "embed":
        files = [Path(path) for path in sorted(glob.glob(args.input_glob))]
        if not files:
            raise ValueError("No cluster files matched the input pattern.")
        args.output_dir.mkdir(parents=True, exist_ok=True)
        for path in files:
            write_table(embed_cluster(read_table(path), args.random_state), args.output_dir / path.name)
    elif args.command == "optimize":
        predictors, target = load_xy(args)
        write_json(
            optimize_regressors(predictors, target, args.n_trials, args.random_state), args.output
        )
    elif args.command == "importance":
        predictors, target = load_xy(args)
        write_table(
            feature_importance(
                predictors, target, read_parameters(args.parameters), args.random_state
            ),
            args.output,
        )
    elif args.command == "validate":
        predictors, target = load_xy(args)
        write_json(
            validate_classifiers(
                predictors,
                target,
                read_parameters(args.parameters),
                args.test_size,
                args.random_state,
            ),
            args.output,
        )
    elif args.command == "shap":
        predictors, target = load_xy(args)
        parameter_sets = read_parameters(args.parameters)
        parameters = parameter_sets["gradient_boosting"].get(
            "parameters", parameter_sets["gradient_boosting"]
        )
        write_table(
            shap_importance(predictors, target, parameters, args.random_state), args.output
        )


if __name__ == "__main__":
    main()

