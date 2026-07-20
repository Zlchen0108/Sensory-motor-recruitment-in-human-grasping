"""Summarize a prespecified consequent across mined association-rule tables."""

from __future__ import annotations

import argparse
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

from publication_utils import configure_matplotlib, save_figure, software_versions, write_json


configure_matplotlib()


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Summarize prespecified association rules.")
    parser.add_argument("--rules-root", type=Path, required=True)
    parser.add_argument("--target-consequents", nargs="+", required=True)
    parser.add_argument("--output-dir", type=Path, default=Path("targeted_rule_summary"))
    parser.add_argument("--singleton-antecedents", action="store_true")
    return parser.parse_args()


def parse_itemset(text: object) -> frozenset[str]:
    return frozenset(part.strip() for part in str(text).split("|") if part.strip())


def main() -> None:
    args = parse_args()
    target = frozenset(args.target_consequents)
    selected: list[pd.DataFrame] = []
    files = sorted(args.rules_root.rglob("association_rules.csv"))
    if not files:
        raise FileNotFoundError(f"No association_rules.csv files found under {args.rules_root}.")
    for path in files:
        rules = pd.read_csv(path)
        required = {"antecedents", "consequents", "support", "confidence", "lift"}
        if not required.issubset(rules.columns):
            raise ValueError(f"Missing required columns in {path}: {sorted(required - set(rules.columns))}")
        mask = rules["consequents"].map(parse_itemset).eq(target)
        if args.singleton_antecedents:
            mask &= rules["antecedents"].map(parse_itemset).map(len).eq(1)
        subset = rules.loc[mask, ["antecedents", "consequents", "support", "confidence", "lift"]].copy()
        subset.insert(0, "dataset", path.parent.name)
        selected.append(subset)

    long_data = pd.concat(selected, ignore_index=True)
    if long_data.empty:
        raise ValueError("No rules matched the prespecified consequent.")
    summary = (
        long_data.groupby("antecedents", as_index=False)
        .agg(
            datasets=("dataset", "nunique"),
            mean_support=("support", "mean"),
            sd_support=("support", "std"),
            mean_confidence=("confidence", "mean"),
            sd_confidence=("confidence", "std"),
            mean_lift=("lift", "mean"),
            sd_lift=("lift", "std"),
        )
        .sort_values("mean_confidence", ascending=False)
    )
    args.output_dir.mkdir(parents=True, exist_ok=True)
    long_data.to_csv(args.output_dir / "targeted_rules_long.csv", index=False)
    summary.to_csv(args.output_dir / "targeted_rules_summary.csv", index=False)
    with pd.ExcelWriter(args.output_dir / "targeted_rule_source_data.xlsx") as writer:
        long_data.to_excel(writer, sheet_name="Dataset_level_rules", index=False)
        summary.to_excel(writer, sheet_name="Summary", index=False)

    figure_height = max(2.4, min(7.0, 0.24 * len(summary)))
    figure, axis = plt.subplots(figsize=(3.5, figure_height))
    positions = np.arange(len(summary))
    axis.errorbar(
        summary["mean_confidence"],
        positions,
        xerr=summary["sd_confidence"].fillna(0),
        fmt="o",
        color="#3B6FB6",
        ecolor="#777777",
        markersize=3,
        capsize=2,
        linewidth=0.8,
    )
    axis.set_yticks(positions, labels=summary["antecedents"])
    axis.invert_yaxis()
    axis.set_xlim(0, 1)
    axis.set_xlabel("Confidence (mean ± s.d.)")
    axis.set_ylabel("Antecedent")
    figure.tight_layout()
    save_figure(figure, args.output_dir / "targeted_rule_confidence")
    plt.close(figure)
    write_json(
        {
            "analysis": "Descriptive summary of a prespecified consequent",
            "target_consequents": sorted(target),
            "singleton_antecedents_only": args.singleton_antecedents,
            "rule_files_examined": len(files),
            "matched_rule_rows": len(long_data),
            "dispersion": "Sample standard deviation across datasets containing each rule",
            "missing_rule_policy": "Absent rules are not treated as zero",
            "software_versions": software_versions(
                ["matplotlib", "numpy", "openpyxl", "pandas"]
            ),
        },
        args.output_dir / "analysis_manifest.json",
    )


if __name__ == "__main__":
    main()
