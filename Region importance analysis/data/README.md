# Data inventory

No study data are committed to this repository.

## Expected inputs

### Point-cloud workbooks

Each workbook must contain at least three numeric columns representing the X, Y, and Z coordinates. Header or instrument-metadata rows can be skipped with the clustering command's `--skip-rows` option.

### Analysis workbooks

Feature-importance, optimization, SHAP, and validation commands expect a rectangular table with one row per observation, numeric predictor columns, and one target column. The target defaults to the final column and can be set explicitly with `--target-column`.

## Required manuscript mapping

Before submission, document the following for every dataset:

| Dataset | Repository or access route | Identifier | Manuscript result | Licence or restriction |
|---|---|---|---|---|
| Point-cloud source data | To be confirmed | To be confirmed | To be confirmed | To be confirmed |
| Processed model inputs | To be confirmed | To be confirmed | To be confirmed | To be confirmed |
| Figure and table source data | To be confirmed | To be confirmed | To be confirmed | To be confirmed |

Do not commit identifiable participant data, credentials, private reviewer URLs, or files whose third-party licence prohibits redistribution.

