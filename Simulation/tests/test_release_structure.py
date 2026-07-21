from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


def test_primary_entry_points_exist():
    required = [
        ROOT / "grasp_generation/scripts/generate_grasps.py",
        ROOT / "grasp_generation/scripts/validate_grasps.py",
        ROOT / "grasp_generation/utils/energy.py",
        ROOT / "grasp_generation/utils/optimizer.py",
        ROOT / "grasp_generation/mjcf/contact_points.json",
        ROOT / "grasp_generation/mjcf/penetration_points.json",
    ]
    assert all(path.is_file() for path in required)


def test_release_metadata_exists():
    required = [
        ROOT / "README.md",
        ROOT / "CODE_AVAILABILITY.md",
        ROOT / "CITATION.cff",
        ROOT / "environment.yml",
        ROOT / "data/README.md",
        ROOT / "RELEASE_SCOPE.md",
    ]
    assert all(path.is_file() for path in required)


def test_primary_generation_script_has_worker_specific_seed():
    source = (ROOT / "grasp_generation/scripts/generate_grasps.py").read_text(encoding="utf-8")
    assert "worker_seed = args.seed + id" in source
    assert "np.random.seed(worker_seed)" in source
    assert "torch.manual_seed(worker_seed)" in source

