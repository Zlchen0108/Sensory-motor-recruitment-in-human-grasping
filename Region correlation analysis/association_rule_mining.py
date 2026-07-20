"""Mine reproducible association rules from explicitly binarized event data."""

from __future__ import annotations

import argparse
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from mlxtend.frequent_patterns import apriori, association_rules

from publication_utils import (
    configure_matplotlib,
    itemset_to_text,
    save_figure,
    sha256,
    software_versions,
    write_json,
)


configure_matplotlib()


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Mine association rules from event tables.")
    parser.add_argument("--input-root", type=Path, required=True)
    parser.add_argument("--output-dir", type=Path, default=Path("association_rule_results"))
    parser.add_argument("--groups", nargs="+", default=["A", "B", "C", "D", "E", "H", "J"])
    parser.add_argument("--sessions", nargs="+", type=int, default=[1, 2, 3])
    parser.add_argument(
        "--strata",
        nargs="*",
        default=[],
        help="Optional strata, for example Power Intermediate Precision.",
    )
    parser.add_argument(
        "--input-template",
        default="Location_Random_Samples.xlsx",
        help="Filename template; may contain {stratum}.",
    )
    parser.add_argument("--exclude-columns", nargs="*", default=[])
    parser.add_argument(
        "--event-threshold",
        type=float,
        default=-1.0,
        help="Numeric values strictly above this threshold are treated as events.",
    )
    parser.add_argument("--min-support", type=float, default=0.20)
    parser.add_argument("--min-confidence", type=float, default=0.20)
    parser.add_argument("--max-itemset-size", type=int, default=None)
    return parser.parse_args()


def load_events(path: Path, excluded: list[str], threshold: float) -> tuple[pd.DataFrame, dict[str, object]]:
    raw = pd.read_excel(path)
    absent = sorted(set(excluded) - set(raw.columns))
    if absent:
        raise ValueError(f"Requested exclusion columns are absent in {path}: {absent}")
    data = raw.drop(columns=excluded) if excluded else raw.copy()
    if data.columns.duplicated().any():
        raise ValueError(f"Duplicate column names in {path}.")
    if data.isna().any().any():
        missing = {str(key): int(value) for key, value in data.isna().sum().items() if value}
        raise ValueError(f"Missing values must be resolved before binarization in {path}: {missing}")

    nonnumeric = [column for column in data.columns if not pd.api.types.is_numeric_dtype(data[column])]
    if nonnumeric:
        invalid = [column for column in nonnumeric if not pd.api.types.is_bool_dtype(data[column])]
        if invalid:
            raise ValueError(f"Non-numeric, non-Boolean analysis columns in {path}: {invalid}")
    events = pd.DataFrame(index=data.index)
    for column in data.columns:
        events[str(column)] = data[column].astype(bool) if pd.api.types.is_bool_dtype(data[column]) else data[column] > threshold
    if len(events) == 0 or events.shape[1] < 2:
        raise ValueError(f"At least one transaction and two event columns are required in {path}.")
    audit = {
        "input_file": str(path.resolve()),
        "sha256": sha256(path),
        "transactions": int(len(events)),
        "event_columns": int(events.shape[1]),
        "excluded_columns": excluded,
        "event_threshold": threshold,
        "event_prevalence": {str(key): float(value) for key, value in events.mean().items()},
    }
    return events, audit


def mine_rules(
    events: pd.DataFrame,
    min_support: float,
    min_confidence: float,
    max_itemset_size: int | None,
) -> tuple[pd.DataFrame, pd.DataFrame]:
    itemsets = apriori(
        events,
        min_support=min_support,
        use_colnames=True,
        max_len=max_itemset_size,
        low_memory=False,
    )
    if itemsets.empty:
        return itemsets, pd.DataFrame()
    rules = association_rules(itemsets, metric="confidence", min_threshold=min_confidence)
    if not rules.empty:
        rules = rules.sort_values(["confidence", "lift", "support"], ascending=False).reset_index(drop=True)
    return itemsets.sort_values("support", ascending=False).reset_index(drop=True), rules


def serialize_itemsets(frame: pd.DataFrame) -> pd.DataFrame:
    result = frame.copy()
    for column in ("itemsets", "antecedents", "consequents"):
        if column in result:
            result[column] = result[column].map(itemset_to_text)
    return result


def plot_rules(rules: pd.DataFrame, output_stem: Path) -> None:
    if rules.empty:
        return
    figure, axis = plt.subplots(figsize=(3.5, 3.0))
    points = axis.scatter(
        rules["support"],
        rules["confidence"],
        c=rules["lift"],
        cmap="viridis",
        s=14,
        alpha=0.75,
        edgecolors="none",
    )
    axis.set_xlabel("Support")
    axis.set_ylabel("Confidence")
    axis.set_xlim(0, 1)
    axis.set_ylim(0, 1)
    colorbar = figure.colorbar(points, ax=axis)
    colorbar.set_label("Lift")
    figure.tight_layout()
    save_figure(figure, output_stem)
    plt.close(figure)


def main() -> None:
    args = parse_args()
    for name, value in (("min-support", args.min_support), ("min-confidence", args.min_confidence)):
        if not 0 < value <= 1:
            raise ValueError(f"--{name} must lie in (0, 1].")
    strata: list[str | None] = args.strata or [None]
    manifest: dict[str, object] = {
        "method": "Apriori frequent-itemset mining followed by confidence-filtered association rules",
        "min_support": args.min_support,
        "min_confidence": args.min_confidence,
        "max_itemset_size": args.max_itemset_size,
        "software_versions": software_versions(
            ["matplotlib", "mlxtend", "numpy", "openpyxl", "pandas"]
        ),
        "datasets": [],
    }

    for group in args.groups:
        for session in args.sessions:
            for stratum in strata:
                filename = args.input_template.format(stratum=stratum or "")
                path = args.input_root / group / f"{group}_{session}" / filename
                if not path.is_file():
                    raise FileNotFoundError(f"Input workbook not found: {path}")
                events, audit = load_events(path, args.exclude_columns, args.event_threshold)
                itemsets, rules = mine_rules(
                    events, args.min_support, args.min_confidence, args.max_itemset_size
                )
                dataset_name = f"{group}_{session}" + (f"_{stratum}" if stratum else "")
                dataset_dir = args.output_dir / dataset_name
                dataset_dir.mkdir(parents=True, exist_ok=True)
                serialized_itemsets = serialize_itemsets(itemsets)
                serialized_rules = serialize_itemsets(rules)
                serialized_itemsets.to_csv(dataset_dir / "frequent_itemsets.csv", index=False)
                serialized_rules.to_csv(dataset_dir / "association_rules.csv", index=False)
                with pd.ExcelWriter(dataset_dir / "association_rule_source_data.xlsx") as writer:
                    serialized_itemsets.to_excel(writer, sheet_name="Frequent_itemsets", index=False)
                    serialized_rules.to_excel(writer, sheet_name="Association_rules", index=False)
                plot_rules(rules, dataset_dir / "association_rule_landscape")
                manifest["datasets"].append(
                    {
                        "dataset": dataset_name,
                        **audit,
                        "frequent_itemsets": int(len(itemsets)),
                        "association_rules": int(len(rules)),
                    }
                )
                print(f"{dataset_name}: {len(itemsets)} itemsets, {len(rules)} rules")

    args.output_dir.mkdir(parents=True, exist_ok=True)
    write_json(manifest, args.output_dir / "analysis_manifest.json")


if __name__ == "__main__":
    main()
