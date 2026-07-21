# Degree-of-Freedom Allocation Probability

This repository provides the analysis code used to calculate normalized degree-of-freedom allocation scores for multiple robotic-hand configurations from single-area and multi-area association and correlation rule tables.

## Repository structure

```text
.
|-- src/dof_probability/   Reusable calculation package and hand profiles
|-- tests/                 Automated unit and command-line tests
|-- data/README.md         Input schema and manuscript-data mapping
|-- outputs/README.md      Generated-output policy
|-- pyproject.toml         Python dependencies and command-line entry point
|-- CITATION.cff           Software citation metadata
`-- CODE_AVAILABILITY.md   Reviewer-facing availability statement
```

## Installation

Python 3.10 or newer is recommended.

```bash
python -m venv .venv
python -m pip install --upgrade pip
python -m pip install -e .
```

For development and testing:

```bash
python -m pip install -e ".[test]"
pytest
```

## Usage

List the available robotic-hand profiles:

```bash
dof-probability list-profiles
```

Run the calculation:

```bash
dof-probability calculate \
  --profile reference_21dof \
  --multi-association data/processed/multi_area_association.xlsx \
  --multi-correlation data/processed/multi_area_correlation.xlsx \
  --single-association data/processed/single_area_association.xlsx \
  --single-correlation data/processed/single_area_correlation.xlsx \
  --output outputs/reference_21dof_scores.csv
```

The four input workbooks must contain a `consequents` column and the rule columns required by the selected profile. The calculation uses the absolute association and correlation values, applies the profile-specific multiplier and distance weights, sums each row, and normalizes the resulting joint scores to the interval [0, 1].

## Reproducibility safeguards

- All eight legacy configurations are stored in a versioned, machine-readable profile file.
- User-specific absolute paths have been removed.
- Input schemas and required columns are validated before calculation.
- Constant results are handled explicitly instead of producing undefined values.
- Results include the profile name, joint identifier, raw score, and normalized score.
- The package has no random component.

## Data and privacy

No study data are committed to this repository. See `data/README.md` for the required schema and the unresolved repository mapping. Do not commit participant-level, confidential, or third-party-restricted data.

## Citation and licence

Update the placeholders in `CITATION.cff` before release. A software licence has not been selected because the authors must confirm ownership and institutional requirements. Add an approved `LICENSE` file before making the repository public.

