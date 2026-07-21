"""Command-line interface for the sensor-validation workflow."""

from __future__ import annotations

import argparse
import glob
from pathlib import Path

from .deep import train_deep_model
from .io import read_table, write_json, write_table
from .models import MODEL_NAMES, evaluate_models, optimize_tree_models, split_xy
from .preprocessing import SENSOR_MASKS, apply_sensor_mask, prepare_recordings


def add_analysis_input(parser: argparse.ArgumentParser) -> None:
    parser.add_argument("--input", type=Path, required=True)
    parser.add_argument("--target-column", default="label")
    parser.add_argument("--test-size", type=float, default=0.2)
    parser.add_argument("--random-state", type=int, default=42)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(prog="sensor-validation")
    commands = parser.add_subparsers(dest="command", required=True)
    prepare = commands.add_parser("prepare", help="Prepare and combine raw recordings.")
    prepare.add_argument("--input-glob", required=True)
    prepare.add_argument("--output", type=Path, required=True)
    prepare.add_argument("--window-size", type=int, default=12)
    prepare.add_argument("--drop-leading-columns", type=int, default=2)
    prepare.add_argument("--label-column", default="label")
    mask = commands.add_parser("mask", help="Apply a predefined sensor ablation mask.")
    mask.add_argument("--input", type=Path, required=True)
    mask.add_argument("--mask", choices=sorted(SENSOR_MASKS), required=True)
    mask.add_argument("--output", type=Path, required=True)
    evaluate = commands.add_parser("evaluate", help="Evaluate conventional classifiers.")
    add_analysis_input(evaluate)
    evaluate.add_argument("--models", nargs="+", choices=MODEL_NAMES, default=list(MODEL_NAMES))
    evaluate.add_argument("--output-dir", type=Path, required=True)
    optimize = commands.add_parser("optimize", help="Optimize tree classifiers.")
    add_analysis_input(optimize)
    optimize.add_argument("--n-trials", type=int, default=100)
    optimize.add_argument("--output", type=Path, required=True)
    deep = commands.add_parser("deep", help="Train a BiLSTM or 1D CNN.")
    add_analysis_input(deep)
    deep.add_argument("--architecture", choices=["bilstm", "cnn1d"], required=True)
    deep.add_argument("--epochs", type=int, default=50)
    deep.add_argument("--batch-size", type=int, default=64)
    deep.add_argument("--learning-rate", type=float, default=0.001)
    deep.add_argument("--output-dir", type=Path, required=True)
    return parser


def main() -> None:
    args = build_parser().parse_args()
    if args.command == "prepare":
        paths = [Path(value) for value in glob.glob(args.input_glob)]
        result = prepare_recordings(paths, args.window_size, args.drop_leading_columns, args.label_column)
        write_table(result, args.output)
    elif args.command == "mask":
        write_table(apply_sensor_mask(read_table(args.input), args.mask), args.output)
    elif args.command == "evaluate":
        predictors, target = split_xy(read_table(args.input), args.target_column)
        summary, tables = evaluate_models(
            predictors, target, args.models, args.test_size, args.random_state
        )
        write_json(summary, args.output_dir / "metrics.json")
        for name, table in tables.items():
            write_table(table, args.output_dir / f"{name}.csv", index=True)
    elif args.command == "optimize":
        predictors, target = split_xy(read_table(args.input), args.target_column)
        write_json(
            optimize_tree_models(predictors, target, args.n_trials, args.random_state), args.output
        )
    elif args.command == "deep":
        predictors, target = split_xy(read_table(args.input), args.target_column)
        summary, tables = train_deep_model(
            predictors, target, args.architecture, args.epochs, args.batch_size,
            args.learning_rate, args.test_size, args.random_state,
        )
        write_json(summary, args.output_dir / "metrics.json")
        for name, table in tables.items():
            write_table(table, args.output_dir / f"{name}.csv", index=True)


if __name__ == "__main__":
    main()

