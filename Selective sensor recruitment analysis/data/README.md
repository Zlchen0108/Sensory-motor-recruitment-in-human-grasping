# Data inventory

No study data are committed to this repository.

## Raw recordings

The preparation command expects one CSV or Excel table per recording. Files are processed in natural numeric filename order. The implemented legacy workflow used recordings 1 through 52, extracted a centered 12-row window, removed the first two metadata columns, and assigned the numeric filename stem as the class label.

For repository release, use English sensor column names such as `channel_1`, `channel_2`, and so on. Preserve sensor units in a separate data dictionary or in unambiguous column names.

## Processed model table

The processed table must contain:

- one row per observation;
- numeric sensor-feature columns;
- a target column, named `label` by default;
- no participant identifiers, local paths, or hidden index columns.

## Required manuscript mapping

| Dataset | Repository or access route | Identifier | Manuscript result | Licence or restriction |
|---|---|---|---|---|
| Raw tactile recordings | To be confirmed | To be confirmed | To be confirmed | To be confirmed |
| Full-sensor processed table | To be confirmed | To be confirmed | To be confirmed | To be confirmed |
| Sensor-mask processed tables | To be confirmed | To be confirmed | To be confirmed | To be confirmed |
| Train/test partition or partition seed | To be confirmed | To be confirmed | To be confirmed | To be confirmed |
| Figure and table source data | To be confirmed | To be confirmed | To be confirmed | To be confirmed |

Document the acquisition device, sampling rate, force units, filtering, excluded recordings, missing-value policy, sensor-to-hand-location mapping, and exact manuscript figure or table supported by each file.

