# Region clustering and t-SNE visualization

This directory contains a self-contained, English-only workflow for exploratory
region clustering. All paths and scientific choices are command-line arguments.
Each run exports source-data tables and a JSON manifest containing input-file
hashes, sample counts, parameters, random seeds, and software versions.

## Seeded K-means clustering

```powershell
python cluster_point_clouds.py `
  --input-root "A:\path\to\Digital_Data" `
  --group A `
  --coordinate-set 2 `
  --clusters 15 `
  --axis-labels "X position (mm)" "Y position (mm)" "Z position (mm)"
```

The default input template is
`{group}/{file_index}/original_points_{file_index}_{coordinate_set}.xlsx`.
Use `--input-template` when the source layout differs. The first eight data rows
are excluded by default to reproduce the legacy workflow; this choice must be
scientifically verified and reported. Prefer named coordinate columns via
`--coordinate-columns` rather than relying on the first three columns.

Coordinates are standardized before K-means. The algorithm uses a fixed seed and
20 initializations. The requested cluster number is not automatically selected
from the elbow curve. If outlier filtering is required, enable it explicitly with
`--outlier-z-threshold` and justify the threshold in the Methods.

## Joint t-SNE embedding

```powershell
python tsne_embedding.py Cluster_0.xlsx Cluster_1.xlsx `
  --source-labels "Region 1" "Region 2" `
  --feature-columns X Y Z `
  --metadata-columns file_number `
  --components 2 `
  --perplexity 30 `
  --max-iter 1000 `
  --seed 20260720
```

All files are embedded jointly so coordinates are comparable within the same
run. Features are standardized before embedding. t-SNE is exploratory only:
apparent distances, gaps, or clusters must not be presented as inferential
evidence, and robustness to seeds and perplexities should be shown if the visual
pattern supports a central claim.

## Review and figure requirements

- Define the experimental unit and report all exact sample counts.
- Report why the first rows were excluded and whether outliers were removed.
- State feature units, standardization, requested cluster count, seed, and `n_init`.
- Report t-SNE perplexity, iteration count, seed, and feature preprocessing.
- Validate clustering with prespecified external criteria or stability analyses.
- Archive scripts, manifests, source data, and the exact dependency environment.

Figures use 5–7 pt Arial/Helvetica-compatible text, labelled axes, editable SVG
and PDF output, a 300-dpi PNG preview, and 600-dpi TIFF output. The default `a.u.` labels must be
replaced when calibrated physical units are known.
