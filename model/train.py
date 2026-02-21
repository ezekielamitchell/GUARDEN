"""
GUARDEN — Model Training Pipeline
Downloads Roboflow dataset, fine-tunes MobileNetV2, exports to .h5

Usage:
    python3 model/train.py --dataset /path/to/dataset --epochs 30 --output model/
"""

import argparse
import os

import numpy as np
import tensorflow as tf
from tensorflow.keras import layers, models
from tensorflow.keras.applications import MobileNetV2
from tensorflow.keras.callbacks import EarlyStopping, ModelCheckpoint, ReduceLROnPlateau
from tensorflow.keras.preprocessing.image import ImageDataGenerator

# ─── Config ───────────────────────────────────────────────────────────────────
IMG_SIZE    = 96      # Must match MODEL_INPUT_W/H in config.h
BATCH_SIZE  = 32
NUM_CLASSES = 2       # [no_rat, rat]


def build_model() -> tf.keras.Model:
    """MobileNetV2 with custom head for binary rat detection."""
    base = MobileNetV2(
        input_shape=(IMG_SIZE, IMG_SIZE, 1),  # Grayscale — 1 channel
        include_top=False,
        weights=None,  # No pretrained weights for grayscale; train from scratch
        alpha=0.35,    # Smallest MobileNetV2 variant — fits in ESP32-C3 RAM
    )

    # Unfreeze all layers for fine-tuning
    base.trainable = True

    model = models.Sequential([
        base,
        layers.GlobalAveragePooling2D(),
        layers.Dropout(0.3),
        layers.Dense(NUM_CLASSES, activation="softmax"),
    ])

    model.compile(
        optimizer=tf.keras.optimizers.Adam(1e-4),
        loss="categorical_crossentropy",
        metrics=["accuracy"],
    )

    model.summary()
    return model


def get_data_generators(dataset_path: str):
    """Creates train/val generators with augmentation."""
    train_datagen = ImageDataGenerator(
        rescale=1.0 / 255,
        validation_split=0.2,
        rotation_range=15,
        width_shift_range=0.1,
        height_shift_range=0.1,
        horizontal_flip=True,
        brightness_range=[0.7, 1.3],  # Simulate day/night lighting
        zoom_range=0.1,
        color_mode="grayscale",
    )

    val_datagen = ImageDataGenerator(
        rescale=1.0 / 255,
        validation_split=0.2,
        color_mode="grayscale",
    )

    train_gen = train_datagen.flow_from_directory(
        dataset_path,
        target_size=(IMG_SIZE, IMG_SIZE),
        batch_size=BATCH_SIZE,
        class_mode="categorical",
        subset="training",
        shuffle=True,
        color_mode="grayscale",
    )

    val_gen = val_datagen.flow_from_directory(
        dataset_path,
        target_size=(IMG_SIZE, IMG_SIZE),
        batch_size=BATCH_SIZE,
        class_mode="categorical",
        subset="validation",
        shuffle=False,
        color_mode="grayscale",
    )

    print(f"Classes: {train_gen.class_indices}")
    return train_gen, val_gen


def train(dataset_path: str, epochs: int, output_dir: str) -> str:
    os.makedirs(output_dir, exist_ok=True)
    h5_path = os.path.join(output_dir, "guarden_v1.h5")

    model = build_model()
    train_gen, val_gen = get_data_generators(dataset_path)

    callbacks = [
        EarlyStopping(patience=5, restore_best_weights=True, monitor="val_accuracy"),
        ModelCheckpoint(h5_path, save_best_only=True, monitor="val_accuracy"),
        ReduceLROnPlateau(factor=0.5, patience=3, monitor="val_loss"),
    ]

    history = model.fit(
        train_gen,
        validation_data=val_gen,
        epochs=epochs,
        callbacks=callbacks,
    )

    val_acc = max(history.history["val_accuracy"])
    print(f"\n✓ Training complete | Best val accuracy: {val_acc:.2%}")
    print(f"✓ Model saved: {h5_path}")

    if val_acc < 0.65:
        print("⚠ Warning: Accuracy below 65% target — consider more data or epochs")

    return h5_path


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Train GUARDEN rat detection model")
    parser.add_argument("--dataset", required=True, help="Path to dataset directory (class subdirs)")
    parser.add_argument("--epochs",  type=int, default=30)
    parser.add_argument("--output",  default="model/")
    args = parser.parse_args()

    train(args.dataset, args.epochs, args.output)
