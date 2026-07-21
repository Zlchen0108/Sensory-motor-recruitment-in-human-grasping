# Peer-review release scope

## Included

- Primary grasp-generation and validation entry points under `grasp_generation/scripts/`.
- Canonical energy, initialization, optimizer, hand-model, object-model, logging, and geometry utilities.
- Required MJCF, MANO, and additional robot-hand assets retained in the curated source tree.
- Object asset-processing utilities.
- The modified `pytorch_kinematics` dependency under `thirdparty/`.
- Curated NumPy and Excel experiment records under `data/`.
- A small example-object bundle under `grasp_generation/example_objects/`.
- Documentation, environment metadata, citation metadata, and structure tests.

## Removed from the reviewer release

- Local GPU preview files and logs.
- Generated convex-decomposition logs containing workstation-specific paths.
- Exploratory scripts with local absolute paths and undocumented analysis state.
- Duplicate copies of dependencies and development modules.
- TorchSDF build output and copied source tree; install the exact external version documented for the manuscript environment.

The removed local materials are preserved outside this repository in `Simulation_local_backup_20260721`; that directory is not part of the upload.

## Blocking release checks

- Confirm that the removed exploratory scripts are not the only implementation of any manuscript result.
- Convert any required exploratory analysis into an English, parameterized script before public release.
- Confirm third-party licences and source citations.
- Record the exact upstream commit and provide a study-specific change log.
- Map every included dataset and asset family to the manuscript figures and tables.
