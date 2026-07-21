"""Command-line interface for the allocation calculation."""

from __future__ import annotations

import argparse
from pathlib import Path

from .core import calculate_scores, load_profiles, read_table, write_table


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(prog="dof-probability")
    commands = parser.add_subparsers(dest="command", required=True)
    commands.add_parser("list-profiles", help="List available hand configurations.")
    calculate = commands.add_parser("calculate", help="Calculate normalized joint scores.")
    calculate.add_argument("--profile", required=True)
    calculate.add_argument("--multi-association", type=Path, required=True)
    calculate.add_argument("--multi-correlation", type=Path, required=True)
    calculate.add_argument("--single-association", type=Path, required=True)
    calculate.add_argument("--single-correlation", type=Path, required=True)
    calculate.add_argument("--output", type=Path, required=True)
    return parser


def main() -> None:
    args = build_parser().parse_args()
    if args.command == "list-profiles":
        for name in sorted(load_profiles()):
            print(name)
        return
    result = calculate_scores(
        args.profile,
        read_table(args.multi_association),
        read_table(args.multi_correlation),
        read_table(args.single_association),
        read_table(args.single_correlation),
    )
    write_table(result, args.output)


if __name__ == "__main__":
    main()

