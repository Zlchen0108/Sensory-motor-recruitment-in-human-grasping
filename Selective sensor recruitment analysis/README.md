# Tactile Sensor Distribution Validation

This repository contains the reproducible analysis workflow used to validate tactile-sensor distribution patterns through data preparation, conventional machine-learning classifiers, tree-model optimization, a bidirectional long short-term memory network, and a one-dimensional convolutional neural network.

## Repository structure

```text
.
|-- src/sensor_validation/   Reusable preparation and modelling package
|-- tests/                   Automated unit and smoke tests
|-- data/README.md           Input schema and manuscript-data mapping
|-- outputs/README.md        Generated-output policy
|-- pyproject.toml           Dependencies and command-line entry point
|-- CITATION.cff             Software citation metadata
`-- CODE_AVAILABILITY.md     Reviewer-facing availability statement
```

## Installation

Python 3.10 or newer is recommended.

```bash
python -m venv .venv
python -m pip install --upgrade pip
python -m pip install -e .
```

Install optional dependencies only when required:

```bash
python -m pip install -e ".[optimization]"
python -m pip install -e ".[deep]"
python -m pip install -e ".[test]"
```

## Data preparation

Prepare numbered CSV or Excel recordings, extract a centered window, remove metadata columns, clamp negative sensor values to zero, add labels, and combine the recordings:

```bash
sensor-validation prepare \
  --input-glob "data/raw/*.csv" \
  --output data/processed/tactile_full.csv \
  --window-size 12 \
  --drop-leading-columns 2
```

Create one of the sensor-ablation datasets used in the original scripts:

```bash
sensor-validation mask \
  --input data/processed/tactile_full.csv \
  --mask reduced_2 \
  --output data/processed/tactile_reduced_2.csv
```

The original script described a 1,000-sample window but implemented a 12-sample window. The command therefore defaults to the implemented value, 12, and exposes it explicitly for manuscript verification.

## Conventional models

Evaluate all conventional classifiers using one shared stratified train/test split:

```bash
sensor-validation evaluate \
  --input data/processed/tactile_full.csv \
  --target-column label \
  --output-dir outputs/conventional
```

Available models are `decision_tree`, `random_forest`, `gradient_boosting`, `svm`, `logistic_regression`, `knn`, and `mlp`. Use `--models` to select a subset. Model-specific metrics, class reports, and confusion matrices are written as machine-readable files.

## Tree-model optimization

```bash
sensor-validation optimize \
  --input data/processed/tactile_full.csv \
  --target-column label \
  --output outputs/optimized_parameters.json \
  --n-trials 100
```

Optimization uses stratified cross-validation and classification accuracy. The legacy script used regressors and mean-squared error for a classification target; this has been corrected.

## Deep-learning models

```bash
sensor-validation deep \
  --architecture bilstm \
  --input data/processed/tactile_full.csv \
  --target-column label \
  --output-dir outputs/bilstm
```

Set `--architecture cnn1d` for the convolutional model. PyTorch is optional, CPU execution is supported, random seeds are fixed, labels remain integer class indices, and the CNN uses adaptive pooling so the fully connected layer is not tied to one feature count.

## Reproducibility safeguards

- All classifiers use the same input table and stratified held-out partition.
- Preprocessing transformations are fitted only on training data through scikit-learn pipelines.
- Models are fitted only on the training partition.
- Random seeds are fixed and recorded in result files.
- Zero-division behaviour in classification metrics is explicit.
- Input tables and class counts are validated before fitting.

## Citation and licence

Update the placeholders in `CITATION.cff` before release. A software licence has not been selected because ownership and institutional requirements must be confirmed by the authors. Add an approved `LICENSE` file before making the repository public.

