import numpy as np
import pandas as pd

from importance_analysis.analysis import embed_cluster, feature_importance, validate_classifiers
from importance_analysis.io import split_predictors_target


PARAMETERS = {
    "decision_tree": {"parameters": {"max_depth": 3}},
    "random_forest": {"parameters": {"n_estimators": 10, "max_depth": 3}},
    "gradient_boosting": {"parameters": {"n_estimators": 10, "max_depth": 2}},
}


def test_split_predictors_target_uses_last_column_by_default():
    frame = pd.DataFrame({"a": [1, 2], "b": [3, 4], "target": [0, 1]})
    predictors, target = split_predictors_target(frame)
    assert list(predictors.columns) == ["a", "b"]
    assert target.name == "target"


def test_embedding_is_bounded_and_preserves_source_ids():
    rng = np.random.default_rng(7)
    frame = pd.DataFrame(rng.normal(size=(20, 3)), columns=["x", "y", "z"])
    frame["source_id"] = np.arange(20)
    result = embed_cluster(frame, random_state=7)
    assert result["tsne_1d"].between(0, 1).all()
    assert result["source_id"].tolist() == frame["source_id"].tolist()


def test_feature_importance_columns_and_order():
    rng = np.random.default_rng(7)
    predictors = pd.DataFrame(rng.normal(size=(40, 3)), columns=list("abc"))
    target = predictors["a"] + rng.normal(scale=0.1, size=40)
    result = feature_importance(predictors, target, PARAMETERS, random_state=7)
    assert set(result.columns) == {
        "feature", "decision_tree", "random_forest", "gradient_boosting", "mean_importance"
    }
    assert result["mean_importance"].is_monotonic_decreasing


def test_validation_reports_all_models():
    rng = np.random.default_rng(7)
    predictors = pd.DataFrame(rng.normal(size=(80, 3)), columns=list("abc"))
    target = pd.Series(np.repeat([0, 1], 40), name="target")
    result = validate_classifiers(predictors, target, PARAMETERS, 0.25, random_state=7)
    assert result["n_train"] == 60
    assert result["n_test"] == 20
    assert set(result["models"]) == set(PARAMETERS)

