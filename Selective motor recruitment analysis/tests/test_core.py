import numpy as np
import pandas as pd
import pytest

from dof_probability.core import CONSEQUENT_ORDER, calculate_scores, load_profiles


def input_tables():
    columns = {
        "TBFE", "IFFE", "IFME", "MFFE", "MFBE", "RFFE", "RFBE", "LFFE",
        "Thumb1", "Thumb", "Index Finger", "Middle Finger", "Ring Finger",
        "Little Finger",
    }
    base = pd.DataFrame({"consequents": CONSEQUENT_ORDER})
    for index, column in enumerate(sorted(columns), start=1):
        base[column] = np.arange(1, 16, dtype=float) * index
    return [base.copy() for _ in range(4)]


def test_all_profiles_have_consistent_shapes():
    for name, profile in load_profiles().items():
        assert len(profile["distance_weights"]) == len(profile["score_terms"]), name
        assert all(len(row) == 15 for row in profile["distance_weights"]), name


def test_reference_profile_returns_bounded_scores():
    result = calculate_scores("reference_21dof", *input_tables())
    assert len(result) == 21
    assert result["normalized_score"].between(0, 1).all()
    assert result["joint_id"].iloc[-1] == "L21"


def test_constant_inputs_do_not_produce_nan():
    tables = input_tables()
    for table in tables:
        for column in table.columns[1:]:
            table[column] = 0.0
    result = calculate_scores("aoyi", *tables)
    assert result["normalized_score"].eq(0).all()
    assert not result.isna().any().any()


def test_missing_columns_are_reported():
    tables = input_tables()
    tables[0] = tables[0].drop(columns=["Thumb"])
    with pytest.raises(ValueError, match="Missing required columns"):
        calculate_scores("reference_21dof", *tables)

