# Reproducible Dexterous-Grasp Simulation

This repository contains the simulation workflow used in this study to generate, inspect, and validate robotic dexterous grasps. It is derived from DexGraspNet and includes study-specific hand assets, curated experiment data, and configuration files. The complete upload directory contains only versioned research materials or standard repository metadata. Caches, local previews, duplicate dependencies, private paths, and non-portable exploratory files are not part of the release.

## Scope and provenance

The grasp-synthesis core is based on DexGraspNet:

> Wang, R. et al. DexGraspNet: A large-scale robotic dexterous grasp dataset for general objects based on simulation. *IEEE International Conference on Robotics and Automation* (2023).

The repository also contains a modified copy of `pytorch_kinematics` under `thirdparty/`. See the upstream project documentation and individual asset licence files before redistribution. Study-specific modifications and assets must be checked by the authors against institutional and third-party licensing requirements before public release.

## Repository structure

```text
.
|-- asset_process/        Object-mesh preparation tools
|-- grasp_generation/    Grasp synthesis and validation workflow
|   |-- scripts/         Primary command-line entry points
|   |-- utils/           Energy, optimization, hand, and object models
|   |-- mjcf/            Hand model and contact-point configuration
|   |-- ManoHand/        MANO-based hand assets
|   `-- open_ai_assets/  Additional robot assets and licence information
|-- thirdparty/           Versioned modified dependency
|-- data/                 Curated NumPy and Excel research data with an inventory
|-- environment.yml      Reproducible base environment specification
|-- CITATION.cff          Software citation metadata
`-- CODE_AVAILABILITY.md Reviewer-facing availability statement
```

## Environment

The original simulation stack requires an NVIDIA GPU and legacy packages constrained by Isaac Gym. The tested manuscript environment must be recorded before release; `environment.yml` captures the known base requirements without claiming versions that were not recoverable from the source folder.

```bash
conda env create -f environment.yml
conda activate dexterous-grasp-simulation
python -m pip install -e thirdparty/pytorch_kinematics
```

Install the journal-analysis environment on a system compatible with the required PyTorch, CUDA, PyTorch3D, TorchSDF, and Isaac Gym versions. TorchSDF and Isaac Gym are not redistributed by this curated release; document their exact versions and official installation routes in the archived release.

## Required data layout

```text
data/
|-- meshdata/       Processed object meshes and surface points
|-- graspdata/      Generated, unvalidated grasps
|-- dataset/        Validated grasp records
`-- experiments/    Small experiment logs and intermediate results
```

The supplied `dataset/` and `experiments/` directories are versioned in this reviewer release. `meshdata/` and newly generated `graspdata/` must be populated when rerunning grasp synthesis with additional object meshes. See `data/README.md` for the current inventory and unresolved provenance fields.

## Grasp generation

Run commands from `grasp_generation` so relative asset paths resolve consistently.

```bash
cd grasp_generation
export CUDA_VISIBLE_DEVICES=0
python scripts/generate_grasps.py \
  --object_code_list OBJECT_ID \
  --data_root_path ../data/meshdata \
  --result_path ../data/graspdata \
  --seed 1 \
  --batch_size_each 32 \
  --n_iter 6000
```

For all objects in the mesh directory:

```bash
python scripts/generate_grasps.py --all --data_root_path ../data/meshdata
```

Each worker derives its seed deterministically from the global seed and worker-task identifier. This avoids identical random streams across parallel tasks while preserving reproducibility.

## Grasp validation

Isaac Gym is required for physics-based validation:

```bash
cd grasp_generation
python scripts/validate_grasps.py
```

The validation scripts require environment-specific asset roots. Replace all local paths with command-line arguments and record the exact simulator build, GPU model, driver, CUDA version, physics engine settings, time step, number of environments, and acceptance thresholds before claiming full reproduction.

## Output format

Generated NumPy files contain a list of grasp dictionaries with object scale, final pose, initial pose, and energy components. Loading legacy `.npy` files may require `allow_pickle=True`; only load files from trusted sources. The README in `grasp_generation/` documents the field names.

## Reproducibility checklist

- Fix and report the global seed.
- Archive the exact code commit and environment specification.
- Record object-mesh identifiers, preprocessing versions, and checksums.
- Report hand model, contact-point set, penetration-point set, energy weights, and thresholds.
- Record GPU/CUDA/PyTorch/PyTorch3D/TorchSDF and Isaac Gym versions.
- Preserve generation logs and validation summaries for the reported figures and tables.
- Map every manuscript result to exact source files.

## Tests

Repository-structure tests do not require the GPU simulation stack:

```bash
python -m pip install pytest
pytest
```

Full numerical and simulator tests require the manuscript environment and the associated mesh assets.

## Citation and licence

Update `CITATION.cff` with the manuscript authors, final repository URL, related article identifier, and archived-release DOI. Do not apply a new licence to upstream code, robot assets, meshes, or generated data until the corresponding rights are confirmed.
