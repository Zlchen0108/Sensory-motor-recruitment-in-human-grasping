# Reproducible Feature-Importance Analysis

This repository contains the analysis code used to cluster three-dimensional point-cloud data, compute one-dimensional t-SNE embeddings, optimize tree-based regression models, estimate feature importance (including SHAP values), and validate classifiers.

## Repository structure

```text
.
|-- src/importance_analysis/   Reusable analysis package
|-- tests/                     Automated smoke tests
|-- data/README.md             Data inventory and expected file format
|-- outputs/README.md          Generated-output policy
|-- pyproject.toml             Dependencies and command-line entry point
|-- CITATION.cff               Software citation metadata
`-- CODE_AVAILABILITY.md       Reviewer-facing code availability statement
```

Input data and generated outputs are intentionally excluded from version control. See `data/README.md` for the required schema.

## Installation

Python 3.10 or newer is recommended.

```bash
python -m venv .venv
python -m pip install --upgrade pip
python -m pip install -e .
```

Install the optional SHAP dependency when SHAP analysis is required:

```bash
python -m pip install -e ".[shap]"
```

For development and testing:

```bash
python -m pip install -e ".[test]"
pytest
```

## Command-line usage

All commands expose additional options through `--help`.

```bash
importance-analysis cluster --input-glob "data/raw/*/original_points_*_3.xlsx" --output-dir outputs/clusters
importance-analysis embed --input-glob "outputs/clusters/cluster_*.xlsx" --output-dir outputs/embeddings
importance-analysis optimize --input data/processed/analysis.xlsx --output outputs/best_parameters.json
importance-analysis importance --input data/processed/analysis.xlsx --parameters outputs/best_parameters.json --output outputs/feature_importance.csv
importance-analysis validate --input data/processed/classification.xlsx --parameters outputs/best_parameters.json --output outputs/validation_metrics.json
importance-analysis shap --input data/processed/analysis.xlsx --parameters outputs/best_parameters.json --output outputs/shap_importance.csv
```

By default, the final column of each analysis workbook is treated as the target. Use `--target-column` to select it explicitly. All relative paths are resolved from the current working directory; no user-specific paths are embedded in the code.

## Reproducibility safeguards

- Random seeds are fixed by default and can be changed with `--random-state`.
- Validation models are fitted only on the training partition.
- Hyperparameter optimization uses shuffled five-fold cross-validation with a fixed seed.
- Parameter files are stored as machine-readable JSON.
- Outputs record the relevant model settings.

## Data and privacy

No participant-level data are included in this repository. Before public release, confirm that the supporting data can be shared under the applicable consent, ethics, institutional, and third-party conditions. For peer review, use an appropriate private repository link or controlled-access route when the data cannot be public.

## Citation and licence

Update the placeholder fields in `CITATION.cff` before release. A software licence has not been selected because ownership and institutional requirements must be confirmed by the authors. Add an approved `LICENSE` file before making the repository public.

