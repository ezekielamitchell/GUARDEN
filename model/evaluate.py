"""
GUARDEN — Model Evaluation
Tests INT8 TFLite model accuracy and latency on a validation set.

Usage:
    python3 model/evaluate.py --model model/guarden_v1_int8.tflite --dataset /path/to/dataset
"""

import argparse
import glob
import os
import time

import numpy as np
import tensorflow as tf
from sklearn.metrics import classification_report, confusion_matrix


IMG_SIZE = 96


def load_dataset(dataset_path: str):
    """Loads validation images and labels from class subdirectories."""
    class_dirs = sorted([
        d for d in os.listdir(dataset_path)
        if os.path.isdir(os.path.join(dataset_path, d))
    ])
    print(f"Classes found: {class_dirs}")

    images, labels = [], []
    for label_idx, class_name in enumerate(class_dirs):
        class_path = os.path.join(dataset_path, class_name)
        paths = glob.glob(os.path.join(class_path, "*.jpg"))
        paths += glob.glob(os.path.join(class_path, "*.png"))

        for path in paths:
            img = tf.keras.preprocessing.image.load_img(
                path, target_size=(IMG_SIZE, IMG_SIZE), color_mode="grayscale"
            )
            arr = tf.keras.preprocessing.image.img_to_array(img)
            arr = (arr / 255.0 * 256 - 128).astype(np.int8)  # INT8 normalize
            images.append(arr)
            labels.append(label_idx)

    return np.array(images), np.array(labels), class_dirs


def evaluate(model_path: str, dataset_path: str) -> None:
    print(f"Loading TFLite model: {model_path}")
    interpreter = tf.lite.Interpreter(model_path=model_path)
    interpreter.allocate_tensors()

    input_details  = interpreter.get_input_details()
    output_details = interpreter.get_output_details()

    print(f"Input:  {input_details[0]['shape']} dtype={input_details[0]['dtype']}")
    print(f"Output: {output_details[0]['shape']} dtype={output_details[0]['dtype']}")

    images, true_labels, class_names = load_dataset(dataset_path)
    print(f"\nEvaluating {len(images)} images...")

    pred_labels = []
    confidences = []
    latencies   = []

    for img in images:
        inp = np.expand_dims(img, axis=0)  # [1, 96, 96, 1]

        t0 = time.perf_counter()
        interpreter.set_tensor(input_details[0]['index'], inp)
        interpreter.invoke()
        latencies.append((time.perf_counter() - t0) * 1000)

        output = interpreter.get_tensor(output_details[0]['index'])[0]

        # Dequantize
        scale      = output_details[0]['quantization_parameters']['scales'][0]
        zero_point = output_details[0]['quantization_parameters']['zero_points'][0]
        scores = (output.astype(np.float32) - zero_point) * scale

        pred_labels.append(np.argmax(scores))
        confidences.append(np.max(scores))

    # ─── Report ───────────────────────────────────────────────────────────────
    accuracy = np.mean(np.array(pred_labels) == true_labels)
    print(f"\n{'='*50}")
    print(f"GUARDEN Model Evaluation Results")
    print(f"{'='*50}")
    print(f"Accuracy:          {accuracy:.2%}  (target: >65%)")
    print(f"Avg confidence:    {np.mean(confidences):.2%}")
    print(f"Avg latency:       {np.mean(latencies):.1f} ms  (target: <1000ms)")
    print(f"Max latency:       {np.max(latencies):.1f} ms")
    print(f"\nClassification Report:")
    print(classification_report(true_labels, pred_labels, target_names=class_names))
    print(f"Confusion Matrix:")
    print(confusion_matrix(true_labels, pred_labels))

    if accuracy < 0.65:
        print("\n⚠ Below 65% target — consider more training data or additional epochs")
    else:
        print(f"\n✓ Accuracy target met: {accuracy:.2%}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Evaluate GUARDEN TFLite model")
    parser.add_argument("--model",   required=True, help="Path to .tflite model")
    parser.add_argument("--dataset", required=True, help="Path to validation dataset")
    args = parser.parse_args()

    evaluate(args.model, args.dataset)
