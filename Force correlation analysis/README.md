# Reproducible correlation analyses

This directory contains manuscript-facing Pearson matrix and Mantel test
workflows. The scripts use explicit inputs, deterministic statistics, source-data
exports, and vector/high-resolution figure outputs suitable for editorial review.

## Pearson matrix analysis

```powershell
python pearson_matrix_analysis.py --input-root "A:\path\to\reordered_files" --exclude-columns file_number
```

The expected layout is `<input-root>/<group>/<condition>/<workbook-name>`. By
default, seven groups (`A B C D E H J`) and three conditions (`C1 C2 C3`) are
analysed. Use `--groups`, `--conditions`, and `--workbook-name` to override them.

Pearson coefficients use pairwise-complete observations with a minimum of three
observations. The 21 independent matrices are combined by an unweighted mean on
Fisher's z scale. This aggregation treats datasets as independent units with
equal weight. If sample sizes or experimental units require different weighting,
the statistical model should be specified before changing the implementation.

Outputs include the pooled matrix, every individual matrix, the number of
contributing datasets per cell, an input-file audit manifest with SHA-256 hashes,
and SVG/PDF/600-dpi TIFF heatmaps.

## Mantel test

```powershell
python mantel_test.py matrix_a.csv matrix_b.csv --permutations 9999 --seed 20260720
```

Input matrices must be symmetric distance matrices with zero diagonals and
identical sample labels. The script reports a two-sided permutation P value with
the finite-permutation correction `(extreme + 1) / (permutations + 1)`. It also
exports the full null distribution and the plotted source relationship.

## Reporting checklist

Report the experimental unit, sample size, missing-data handling, excluded
variables, number of datasets, aggregation rule, permutation count, random seed,
effect size, exact P value, and any multiplicity correction. Pearson correlation
does not require z-score standardization because it is invariant to positive
linear rescaling; transformations used to address distributional assumptions
must instead be scientifically justified and reported.
