"""Core analysis functions shared by the command-line interface."""

from __future__ import annotations

from pathlib import Path
from typing import Any

import numpy as np
import pandas as pd
from scipy import stats
from sklearn.cluster import KMeans
from sklearn.ensemble import (
    GradientBoostingClassifier,
    GradientBoostingRegressor,
    RandomForestClassifier,
    RandomForestRegressor,
)
from sklearn.manifold import TSNE
from sklearn.metrics import accuracy_score, confusion_matrix
from sklearn.model_selection import KFold, cross_val_score, train_test_split
from sklearn.preprocessing import MinMaxScaler
from sklearn.tree import DecisionTreeClassifier, DecisionTreeRegressor


REGRESSORS = {
    "decision_tree": DecisionTreeRegressor,
    "random_forest": RandomForestRegressor,
    "gradient_boosting": GradientBoostingRegressor,
}

CLASSIFIERS = {
    "decision_tree": DecisionTreeClassifier,
    "random_forest": RandomForestClassifier,
    "gradient_boosting": GradientBoostingClassifier,
}


def remove_outliers_zscore(values: np.ndarray, threshold: float = 3.0) -> np.ndarray:
    """Remove rows containing a coordinate beyond the Z-score threshold."""
    scores = np.abs(stats.zscore(values, axis=0, nan_policy="omit"))
    scores = np.nan_to_num(scores, nan=0.0)
    return values[(scores < threshold).all(axis=1)]


def cluster_point_clouds(
    files: list[Path], n_clusters: int, skip_rows: int, random_state: int
) -> tuple[pd.DataFrame, pd.DataFrame]:
    """Combine, filter, and cluster three-dimensional point-cloud workbooks."""
    blocks: list[np.ndarray] = []
    for source_id, path in enumerate(files, start=1):
        frame = pd.read_excel(path)
        points = frame.iloc[skip_rows:, :3].apply(pd.to_numeric, errors="raise").to_numpy()
        points = remove_outliers_zscore(points)
        source = np.full((len(points), 1), source_id)
        blocks.append(np.hstack((points, source)))
    if not blocks:
        raise ValueError("No point-cloud files matched the input pattern.")
    combined = np.vstack(blocks)
    model = KMeans(n_clusters=n_clusters, random_state=random_state, n_init=10)
    labels = model.fit_predict(combined[:, :3])
    points = pd.DataFrame(combined, columns=["x", "y", "z", "source_id"])
    points["cluster"] = labels
    centers = pd.DataFrame(model.cluster_centers_, columns=["x", "y", "z"])
    centers.insert(0, "cluster", np.arange(n_clusters))
    return points, centers


def embed_cluster(frame: pd.DataFrame, random_state: int) -> pd.DataFrame:
    """Create a normalized one-dimensional t-SNE embedding."""
    coordinate_names = [name for name in ("x", "y", "z") if name in frame.columns]
    if len(coordinate_names) != 3:
        coordinate_names = list(frame.columns[:3])
    values = frame[coordinate_names].to_numpy()
    if len(values) < 3:
        raise ValueError("t-SNE requires at least three observations.")
    perplexity = min(30.0, max(2.0, (len(values) - 1) / 3))
    embedded = TSNE(
        n_components=1,
        perplexity=perplexity,
        init="pca",
        learning_rate="auto",
        random_state=random_state,
    ).fit_transform(values)
    normalized = MinMaxScaler().fit_transform(embedded).ravel()
    output = pd.DataFrame({"tsne_1d": normalized})
    if "source_id" in frame.columns:
        output["source_id"] = frame["source_id"].to_numpy()
    return output


def optimize_regressors(
    predictors: pd.DataFrame,
    target: pd.Series,
    n_trials: int,
    random_state: int,
) -> dict[str, dict[str, Any]]:
    """Optimize three tree regressors using reproducible cross-validation."""
    try:
        import optuna
    except ImportError as error:
        raise RuntimeError("Install the project dependencies before optimization.") from error
    cv = KFold(n_splits=5, shuffle=True, random_state=random_state)
    spaces = {
        "decision_tree": lambda trial: {
            "max_depth": trial.suggest_int("max_depth", 3, 30),
            "min_samples_split": trial.suggest_int("min_samples_split", 2, 30),
            "min_samples_leaf": trial.suggest_int("min_samples_leaf", 1, 30),
            "max_features": trial.suggest_float("max_features", 0.2, 1.0),
        },
        "random_forest": lambda trial: {
            "n_estimators": trial.suggest_int("n_estimators", 50, 500),
            "max_depth": trial.suggest_int("max_depth", 3, 30),
            "max_features": trial.suggest_float("max_features", 0.2, 1.0),
        },
        "gradient_boosting": lambda trial: {
            "max_depth": trial.suggest_int("max_depth", 2, 10),
            "learning_rate": trial.suggest_float("learning_rate", 0.001, 0.1, log=True),
            "n_estimators": trial.suggest_int("n_estimators", 50, 300),
            "subsample": trial.suggest_float("subsample", 0.5, 1.0),
        },
    }
    results: dict[str, dict[str, Any]] = {}
    for index, (name, model_class) in enumerate(REGRESSORS.items()):
        def objective(trial: optuna.Trial) -> float:
            parameters = spaces[name](trial)
            model = model_class(**parameters, random_state=random_state)
            scores = cross_val_score(
                model, predictors, target, scoring="neg_mean_squared_error", cv=cv, n_jobs=-1
            )
            return float(scores.mean())

        sampler = optuna.samplers.TPESampler(seed=random_state + index)
        study = optuna.create_study(direction="maximize", sampler=sampler)
        study.optimize(objective, n_trials=n_trials, show_progress_bar=False)
        results[name] = {
            "parameters": study.best_params,
            "mean_negative_mse": study.best_value,
            "random_state": random_state,
        }
    return results


def feature_importance(
    predictors: pd.DataFrame,
    target: pd.Series,
    parameter_sets: dict[str, dict[str, Any]],
    random_state: int,
) -> pd.DataFrame:
    """Fit three regressors and return their native feature importances."""
    output = pd.DataFrame({"feature": predictors.columns})
    columns: list[str] = []
    for name, model_class in REGRESSORS.items():
        parameters = parameter_sets[name].get("parameters", parameter_sets[name])
        model = model_class(**parameters, random_state=random_state)
        model.fit(predictors, target)
        output[name] = model.feature_importances_
        columns.append(name)
    output["mean_importance"] = output[columns].mean(axis=1)
    return output.sort_values("mean_importance", ascending=False, ignore_index=True)


def validate_classifiers(
    predictors: pd.DataFrame,
    target: pd.Series,
    parameter_sets: dict[str, dict[str, Any]],
    test_size: float,
    random_state: int,
) -> dict[str, Any]:
    """Evaluate classifiers on a held-out partition without data leakage."""
    x_train, x_test, y_train, y_test = train_test_split(
        predictors,
        target,
        test_size=test_size,
        random_state=random_state,
        stratify=target,
    )
    labels = sorted(target.unique().tolist())
    results: dict[str, Any] = {
        "random_state": random_state,
        "test_size": test_size,
        "n_train": len(x_train),
        "n_test": len(x_test),
        "labels": labels,
        "models": {},
    }
    for name, model_class in CLASSIFIERS.items():
        parameters = parameter_sets[name].get("parameters", parameter_sets[name])
        model = model_class(**parameters, random_state=random_state)
        model.fit(x_train, y_train)
        predictions = model.predict(x_test)
        matrix = confusion_matrix(y_test, predictions, labels=labels, normalize="true")
        results["models"][name] = {
            "accuracy": float(accuracy_score(y_test, predictions)),
            "normalized_confusion_matrix": matrix.tolist(),
        }
    return results


def shap_importance(
    predictors: pd.DataFrame,
    target: pd.Series,
    parameters: dict[str, Any],
    random_state: int,
) -> pd.DataFrame:
    """Calculate mean absolute SHAP values for a gradient-boosting regressor."""
    try:
        import shap
    except ImportError as error:
        raise RuntimeError('Install the optional dependency with: pip install -e ".[shap]"') from error
    model = GradientBoostingRegressor(**parameters, random_state=random_state)
    model.fit(predictors, target)
    values = shap.Explainer(model)(predictors).values
    means = np.abs(values).mean(axis=0)
    return pd.DataFrame(
        {"feature": predictors.columns, "mean_absolute_shap": means}
    ).sort_values("mean_absolute_shap", ascending=False, ignore_index=True)
