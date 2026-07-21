"""Leakage-safe conventional model evaluation and optimization."""

from __future__ import annotations

from typing import Any

import numpy as np
import pandas as pd
from sklearn.ensemble import GradientBoostingClassifier, RandomForestClassifier
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import accuracy_score, classification_report, confusion_matrix
from sklearn.model_selection import StratifiedKFold, cross_val_score, train_test_split
from sklearn.neighbors import KNeighborsClassifier
from sklearn.neural_network import MLPClassifier
from sklearn.pipeline import make_pipeline
from sklearn.preprocessing import StandardScaler
from sklearn.svm import SVC
from sklearn.tree import DecisionTreeClassifier


MODEL_NAMES = (
    "decision_tree", "random_forest", "gradient_boosting", "svm",
    "logistic_regression", "knn", "mlp",
)


def split_xy(frame: pd.DataFrame, target_column: str) -> tuple[pd.DataFrame, pd.Series]:
    if target_column not in frame.columns:
        raise ValueError(f"Target column not found: {target_column}")
    predictors = frame.drop(columns=[target_column])
    if predictors.empty or not all(pd.api.types.is_numeric_dtype(dtype) for dtype in predictors.dtypes):
        raise ValueError("All predictor columns must be numeric.")
    target = frame[target_column]
    if target.nunique() < 2:
        raise ValueError("At least two target classes are required.")
    if target.value_counts().min() < 2:
        raise ValueError("Each class requires at least two observations for stratified splitting.")
    return predictors, target


def build_model(name: str, random_state: int, parameters: dict[str, Any] | None = None):
    parameters = dict(parameters or {})
    if name == "decision_tree":
        return DecisionTreeClassifier(random_state=random_state, **parameters)
    if name == "random_forest":
        return RandomForestClassifier(random_state=random_state, n_jobs=-1, **parameters)
    if name == "gradient_boosting":
        return GradientBoostingClassifier(random_state=random_state, **parameters)
    if name == "svm":
        return make_pipeline(StandardScaler(), SVC(**parameters))
    if name == "logistic_regression":
        parameters.setdefault("max_iter", 1000)
        return make_pipeline(StandardScaler(), LogisticRegression(random_state=random_state, **parameters))
    if name == "knn":
        return make_pipeline(StandardScaler(), KNeighborsClassifier(**parameters))
    if name == "mlp":
        parameters.setdefault("hidden_layer_sizes", (100,))
        parameters.setdefault("max_iter", 500)
        return make_pipeline(StandardScaler(), MLPClassifier(random_state=random_state, **parameters))
    raise ValueError(f"Unknown model: {name}")


def evaluate_models(
    predictors: pd.DataFrame,
    target: pd.Series,
    names: list[str],
    test_size: float,
    random_state: int,
) -> tuple[dict[str, Any], dict[str, pd.DataFrame]]:
    """Evaluate models using one shared, stratified held-out partition."""
    x_train, x_test, y_train, y_test = train_test_split(
        predictors, target, test_size=test_size, random_state=random_state, stratify=target
    )
    labels = sorted(target.unique().tolist())
    summary: dict[str, Any] = {
        "random_state": random_state,
        "test_size": test_size,
        "n_train": len(x_train),
        "n_test": len(x_test),
        "labels": labels,
        "models": {},
    }
    tables: dict[str, pd.DataFrame] = {}
    for name in names:
        model = build_model(name, random_state)
        model.fit(x_train, y_train)
        predicted = model.predict(x_test)
        summary["models"][name] = {"accuracy": float(accuracy_score(y_test, predicted))}
        tables[f"{name}_confusion_matrix"] = pd.DataFrame(
            confusion_matrix(y_test, predicted, labels=labels), index=labels, columns=labels
        )
        tables[f"{name}_classification_report"] = pd.DataFrame(
            classification_report(y_test, predicted, labels=labels, output_dict=True, zero_division=0)
        ).transpose()
    return summary, tables


def optimize_tree_models(
    predictors: pd.DataFrame, target: pd.Series, n_trials: int, random_state: int
) -> dict[str, Any]:
    """Optimize tree classifiers with seeded TPE and stratified cross-validation."""
    try:
        import optuna
    except ImportError as error:
        raise RuntimeError('Install optimization support with: pip install -e ".[optimization]"') from error
    folds = min(5, int(target.value_counts().min()))
    cv = StratifiedKFold(n_splits=folds, shuffle=True, random_state=random_state)
    spaces = {
        "decision_tree": lambda trial: {
            "max_depth": trial.suggest_int("max_depth", 2, 30),
            "min_samples_split": trial.suggest_int("min_samples_split", 2, 30),
            "min_samples_leaf": trial.suggest_int("min_samples_leaf", 1, 20),
            "max_features": trial.suggest_float("max_features", 0.2, 1.0),
        },
        "random_forest": lambda trial: {
            "n_estimators": trial.suggest_int("n_estimators", 50, 500),
            "max_depth": trial.suggest_int("max_depth", 2, 30),
            "max_features": trial.suggest_float("max_features", 0.2, 1.0),
        },
        "gradient_boosting": lambda trial: {
            "n_estimators": trial.suggest_int("n_estimators", 50, 300),
            "max_depth": trial.suggest_int("max_depth", 2, 10),
            "learning_rate": trial.suggest_float("learning_rate", 0.001, 0.1, log=True),
            "subsample": trial.suggest_float("subsample", 0.5, 1.0),
        },
    }
    results: dict[str, Any] = {}
    for offset, name in enumerate(("decision_tree", "random_forest", "gradient_boosting")):
        def objective(trial):
            params = spaces[name](trial)
            scores = cross_val_score(
                build_model(name, random_state, params), predictors, target,
                scoring="accuracy", cv=cv, n_jobs=-1,
            )
            return float(np.mean(scores))
        study = optuna.create_study(
            direction="maximize", sampler=optuna.samplers.TPESampler(seed=random_state + offset)
        )
        study.optimize(objective, n_trials=n_trials, show_progress_bar=False)
        results[name] = {
            "parameters": study.best_params,
            "mean_cross_validated_accuracy": study.best_value,
            "folds": folds,
        }
    return results

