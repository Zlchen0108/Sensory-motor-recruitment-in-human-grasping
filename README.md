<div align="center">

# Sensory-motor Recruitment in Human Grasping

**Reproducible firmware, simulation, and analysis workflows for investigating regional and selective sensory-motor recruitment during grasping**

[![Research code](https://img.shields.io/badge/type-research%20code-2f6f73)](#repository-overview)
[![Python](https://img.shields.io/badge/Python-3.10%2B-3776ab?logo=python&logoColor=white)](#software-requirements)
[![Firmware](https://img.shields.io/badge/firmware-STM32F103C8-6b7280)](#embedded-firmware)

[Repository overview](#repository-overview) | [Reviewer guide](#reviewer-guide) | [Reproducibility](#reproducibility) | [Citation](#citation)

</div>

---

## Overview

This repository accompanies a study of how sensory inputs and motor degrees of freedom are recruited during human grasping. It brings together the embedded acquisition and control firmware, dexterous-grasp simulation, regional and force-correlation analyses, feature-importance estimation, and selective sensory and motor recruitment validation.

The repository is organized as a collection of self-contained analysis modules. Each manuscript-facing Python module provides its own installation instructions, command-line examples, input-data specification, output policy, and software citation metadata.

> **Review status:** This repository is intended to support transparent peer review and computational reproducibility. Manuscript-specific accession information, the final article citation, and archived release identifiers should be added when available.

## Repository overview

| Module | Purpose | Main documentation |
| --- | --- | --- |
| [`EID-hand-control`](./EID-hand-control/) | STM32/Keil firmware for acquisition and hand-control hardware | Keil project: `Project.uvprojx` |
| [`Simulation`](./Simulation/) | Generation, inspection, and validation of dexterous grasps derived from DexGraspNet | [README](./Simulation/README.md) |
| [`Force correlation analysis`](./Force%20correlation%20analysis/) | Pearson correlation matrices and permutation-based Mantel tests | [README](./Force%20correlation%20analysis/README.md) |
| [`Region correlation analysis`](./Region%20correlation%20analysis/) | Association-rule mining and prespecified regional rule summaries | [README](./Region%20correlation%20analysis/README.md) |
| [`Hand region analysis`](./Hand%20region%20analysis/) | Point-cloud clustering and joint t-SNE visualization of hand regions | [README](./Hand%20region%20analysis/README.md) |
| [`Region importance analysis`](./Region%20importance%20analysis/) | Feature-importance, SHAP, regression, and classifier validation workflows | [README](./Region%20importance%20analysis/README.md) |
| [`Selective motor recruitment analysis`](./Selective%20motor%20recruitment%20analysis/) | Degree-of-freedom allocation analysis for robotic-hand configurations | [README](./Selective%20motor%20recruitment%20analysis/README.md) |
| [`Selective sensor recruitment analysis`](./Selective%20sensor%20recruitment%20analysis/) | Validation of tactile-sensor distributions using conventional and deep-learning models | [README](./Selective%20sensor%20recruitment%20analysis/README.md) |

## Reviewer guide

For an efficient review, we recommend the following order:

1. Read this page for the study-level workflow and module map.
2. Open the README of the module corresponding to the manuscript result being assessed.
3. Review the module's data specification and code-availability statement.
4. Create an isolated environment and install the module locally.
5. Run its automated tests, where supplied.
6. Execute the documented command-line workflow using the manuscript input data.
7. Compare the generated source-data tables, manifests, and figures with the corresponding manuscript outputs.

The three packaged analysis modules below expose installable command-line tools and automated tests:

- `Region importance analysis`
- `Selective motor recruitment analysis`
- `Selective sensor recruitment analysis`

The correlation and hand-region modules are script-based workflows with dedicated `requirements.txt` files and explicit command-line arguments.

## Software requirements

### Python analyses

Python 3.10 or newer is recommended for the packaged analysis modules. Create a separate environment for each module to avoid dependency conflicts:

```bash
python -m venv .venv

# Windows
.venv\Scripts\activate

python -m pip install --upgrade pip
```

For a module containing `pyproject.toml`:

```bash
cd "Region importance analysis"
python -m pip install -e ".[test]"
pytest
```

For a module containing `requirements.txt`:

```bash
cd "Force correlation analysis"
python -m pip install -r requirements.txt
```

Optional machine-learning dependencies are documented in the corresponding module README and should be installed only for the relevant workflow.

### Embedded firmware

The firmware targets an STM32F103C8 microcontroller and is organized as a Keil MDK/uVision project.

1. Open `EID-hand-control/Project.uvprojx` in Keil uVision.
2. Select the documented target configuration.
3. Build the project and resolve hardware-specific programmer settings locally.
4. Flash the target only after confirming pin assignments and connected hardware.

Generated Keil object files, listings, and user-session files are not source code and are not required to understand the firmware implementation.

### Simulation

The simulation workflow uses a separate Conda environment because its legacy GPU and Isaac Gym dependencies differ from the analysis stack:

```bash
cd Simulation
conda env create -f environment.yml
conda activate dexterous-grasp-simulation
python -m pip install -e thirdparty/pytorch_kinematics
```

See the [simulation documentation](./Simulation/README.md) for hardware constraints, upstream provenance, data inventory, and workflow entry points.

## Code provenance

The simulation workflow is derived from **DexGraspNet** and includes study-specific modifications. Third-party software and assets retain their original provenance and licensing information within the relevant directories. Users must review those terms before redistribution or reuse.

The analysis modules developed for this study are documented separately from imported firmware libraries and third-party simulation components. A repository-wide license has not yet been declared; therefore, the presence of source code should not be interpreted as permission for unrestricted reuse.

## Citation

If you use this repository, cite the associated article once its bibliographic details are available. Modules containing a `CITATION.cff` file can also be cited through GitHub's **Cite this repository** interface.

Until the article citation is finalized, refer to the software as:

```text
Zlchen0108. Sensory-motor Recruitment in Human Grasping: research code,
firmware, simulation, and analysis workflows. GitHub repository.
https://github.com/Zlchen0108/Sensory-motor-recruitment-in-human-grasping
```

The simulation component additionally requires citation of the upstream DexGraspNet publication, as described in its module README.

## Reporting issues

When reporting a reproducibility problem, include the module name, command used, operating system, Python or toolchain version, relevant parameter file, and the complete error message. Do not attach participant-identifiable or otherwise restricted research data to a public issue.

---

<div align="center">

Maintained by [Zlchen0108](https://github.com/Zlchen0108)

</div>
