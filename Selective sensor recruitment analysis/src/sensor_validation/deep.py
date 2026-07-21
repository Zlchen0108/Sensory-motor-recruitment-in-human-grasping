"""Optional PyTorch classifiers with deterministic training controls."""

from __future__ import annotations

from typing import Any

import numpy as np
import pandas as pd
from sklearn.metrics import accuracy_score, classification_report, confusion_matrix
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import LabelEncoder, StandardScaler


def train_deep_model(
    predictors: pd.DataFrame,
    target: pd.Series,
    architecture: str,
    epochs: int,
    batch_size: int,
    learning_rate: float,
    test_size: float,
    random_state: int,
) -> tuple[dict[str, Any], dict[str, pd.DataFrame]]:
    """Train and evaluate a BiLSTM or adaptive 1D CNN."""
    try:
        import torch
        from torch import nn
    except ImportError as error:
        raise RuntimeError('Install deep-learning support with: pip install -e ".[deep]"') from error
    if architecture not in {"bilstm", "cnn1d"}:
        raise ValueError(f"Unknown architecture: {architecture}")
    torch.manual_seed(random_state)
    np.random.seed(random_state)
    if torch.cuda.is_available():
        torch.cuda.manual_seed_all(random_state)
    torch.use_deterministic_algorithms(True, warn_only=True)
    encoder = LabelEncoder()
    labels = encoder.fit_transform(target)
    x_train, x_test, y_train, y_test = train_test_split(
        predictors.to_numpy(dtype=np.float32), labels, test_size=test_size,
        random_state=random_state, stratify=labels,
    )
    scaler = StandardScaler().fit(x_train)
    x_train = scaler.transform(x_train).astype(np.float32)
    x_test = scaler.transform(x_test).astype(np.float32)
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    train_x = torch.from_numpy(x_train).to(device)
    test_x = torch.from_numpy(x_test).to(device)
    train_y = torch.from_numpy(y_train).long().to(device)

    class BiLSTM(nn.Module):
        def __init__(self, classes: int):
            super().__init__()
            self.lstm = nn.LSTM(1, 64, bidirectional=True, batch_first=True)
            self.dropout = nn.Dropout(0.5)
            self.output = nn.Linear(128, classes)

        def forward(self, values):
            sequence = values.unsqueeze(-1)
            encoded, _ = self.lstm(sequence)
            return self.output(self.dropout(encoded[:, -1, :]))

    class CNN1D(nn.Module):
        def __init__(self, classes: int):
            super().__init__()
            self.features = nn.Sequential(
                nn.Conv1d(1, 64, kernel_size=3, padding=1), nn.ReLU(), nn.MaxPool1d(2),
                nn.Conv1d(64, 128, kernel_size=3, padding=1), nn.ReLU(),
                nn.AdaptiveAvgPool1d(1),
            )
            self.output = nn.Sequential(nn.Flatten(), nn.Dropout(0.5), nn.Linear(128, classes))

        def forward(self, values):
            return self.output(self.features(values.unsqueeze(1)))

    classes = len(encoder.classes_)
    model = (BiLSTM(classes) if architecture == "bilstm" else CNN1D(classes)).to(device)
    optimizer = torch.optim.Adam(model.parameters(), lr=learning_rate)
    criterion = nn.CrossEntropyLoss()
    generator = torch.Generator().manual_seed(random_state)
    for _ in range(epochs):
        model.train()
        for indices in torch.randperm(len(train_x), generator=generator).split(batch_size):
            indices = indices.to(device)
            optimizer.zero_grad()
            loss = criterion(model(train_x[indices]), train_y[indices])
            loss.backward()
            optimizer.step()
    model.eval()
    with torch.no_grad():
        predicted = model(test_x).argmax(dim=1).cpu().numpy()
    class_labels = list(range(classes))
    summary = {
        "architecture": architecture,
        "accuracy": float(accuracy_score(y_test, predicted)),
        "epochs": epochs,
        "batch_size": batch_size,
        "learning_rate": learning_rate,
        "random_state": random_state,
        "device": str(device),
        "classes": [value.item() if hasattr(value, "item") else value for value in encoder.classes_],
    }
    tables = {
        "confusion_matrix": pd.DataFrame(
            confusion_matrix(y_test, predicted, labels=class_labels),
            index=encoder.classes_, columns=encoder.classes_,
        ),
        "classification_report": pd.DataFrame(
            classification_report(
                y_test, predicted, labels=class_labels, target_names=encoder.classes_.astype(str),
                output_dict=True, zero_division=0,
            )
        ).transpose(),
    }
    return summary, tables
