# Association-rule and exploratory clustering workflows

This directory contains English-only, command-line-driven analyses intended for
transparent peer review. No script contains synthetic manuscript data or
machine-specific input paths. Each analysis writes source-data tables and an
analysis manifest recording parameters, input paths, and SHA-256 file hashes.

## Association-rule mining

```powershell
python association_rule_mining.py `
  --input-root "C:\path\to\Association Rules" `
  --exclude-columns file_number `
  --event-threshold -1 `
  --min-support 0.20 `
  --min-confidence 0.20
```

For stratified files, provide a filename template and strata explicitly:

```powershell
python association_rule_mining.py `
  --input-root "C:\path\to\Association Rules" `
  --strata Power Intermediate Precision `
  --input-template "Location_Random_Samples_For_{stratum}Grasp.xlsx" `
  --exclude-columns file_number
```

The event threshold is a scientific analysis choice, not a formatting setting.
Its meaning and justification must be stated in the Methods. Missing values cause
the analysis to stop rather than being silently classified as absent events.

## Prespecified rule summary

```powershell
python targeted_rule_summary.py `
  --rules-root association_rule_results `
  --target-consequents TBFE TBBE TM `
  --singleton-antecedents
```

The summary reports the mean and sample standard deviation across datasets in
which each rule was discovered. A rule absent after thresholding is not assigned
a value of zero. This selection-dependent descriptive summary must not be
presented as confirmatory inference.

## Point-cloud clustering

```powershell
python cluster_point_clouds.py `
  --input-root "A:\path\to\Digital_Data" `
  --group A `
  --coordinate-set 2 `
  --clusters 15
```

Coordinates are standardized before seeded K-means (`n_init=20`). The requested
number of clusters is never automatically inferred from the elbow curve. Any
outlier exclusion must be explicitly enabled and reported.

## t-SNE embedding

```powershell
python tsne_embedding.py Cluster_0.xlsx Cluster_1.xlsx --perplexity 30 --seed 20260720
```

All input files are embedded jointly so their coordinates are comparable within
one run. t-SNE is used only for exploratory visualization; apparent distances or
clusters are not treated as statistical evidence.

## Figure and reporting contract

Figures use 5–7 pt Arial/Helvetica-compatible text, accessible palettes, editable
SVG/PDF output, and 600-dpi TIFF output. The manuscript must report the
experimental unit, number of transactions or points, event definition, excluded
columns, support and confidence thresholds, software versions, random seeds,
clustering standardization, requested cluster count, and all deviations from the
prespecified workflow. Archive the scripts, manifests, source-data exports, and
dependency lock file used for the final analysis.
