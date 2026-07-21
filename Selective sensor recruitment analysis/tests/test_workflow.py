import numpy as np
import pandas as pd
import pytest

from sensor_validation.models import evaluate_models, split_xy
from sensor_validation.preprocessing import apply_sensor_mask, prepare_recordings


def test_sensor_mask_sets_expected_channels_to_zero():
    frame = pd.DataFrame({f"channel_{index}": [1.0, 2.0] for index in range(1, 49)})
    frame["label"] = [1, 2]
    result = apply_sensor_mask(frame, "reduced_5")
    assert result[["channel_28", "channel_29", "channel_37", "channel_38"]].eq(0).all().all()
    assert result["channel_1"].equals(frame["channel_1"])


def test_prepare_recordings_clamps_and_labels(tmp_path):
    for number in (1, 2):
        frame = pd.DataFrame({"meta_a": range(20), "meta_b": range(20), "channel_1": [-1.0] * 20})
        frame.to_csv(tmp_path / f"{number}.csv", index=False)
    result = prepare_recordings(list(tmp_path.glob("*.csv")), 12, 2, "label")
    assert len(result) == 24
    assert result["channel_1"].eq(0).all()
    assert set(result["label"]) == {1, 2}


def test_evaluation_uses_one_shared_split():
    rng = np.random.default_rng(7)
    predictors = pd.DataFrame(rng.normal(size=(100, 4)), columns=list("abcd"))
    target = pd.Series(np.repeat([0, 1], 50), name="label")
    summary, tables = evaluate_models(
        predictors, target, ["decision_tree", "svm", "logistic_regression"], 0.2, 7
    )
    assert summary["n_train"] == 80
    assert summary["n_test"] == 20
    assert set(summary["models"]) == {"decision_tree", "svm", "logistic_regression"}
    assert "svm_confusion_matrix" in tables


def test_split_xy_rejects_non_numeric_predictors():
    frame = pd.DataFrame({"sensor": ["low", "high", "low", "high"], "label": [0, 1, 0, 1]})
    with pytest.raises(ValueError, match="numeric"):
        split_xy(frame, "label")

