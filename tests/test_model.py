"""
Tests for GUARDEN TFLite model — shape, dtype, output range.
Run: pytest tests/test_model.py -v
Requires: guarden_v1_int8.tflite in model/ directory
"""

import os
import numpy as np
import pytest


MODEL_PATH = os.path.join(os.path.dirname(__file__), "..", "model", "guarden_v1_int8.tflite")


@pytest.fixture(scope="module")
def interpreter():
    """Load TFLite interpreter once for all tests."""
    tf_lite = pytest.importorskip("tensorflow.lite")
    if not os.path.exists(MODEL_PATH):
        pytest.skip("Model file not found — run model/quantize.py first")

    interp = tf_lite.Interpreter(model_path=MODEL_PATH)
    interp.allocate_tensors()
    return interp


class TestModelSpec:
    def test_input_shape(self, interpreter):
        details = interpreter.get_input_details()
        shape = details[0]["shape"]
        assert shape[1] == 96, "Input height must be 96"
        assert shape[2] == 96, "Input width must be 96"
        assert shape[3] == 1,  "Input channels must be 1 (grayscale)"

    def test_input_dtype_int8(self, interpreter):
        details = interpreter.get_input_details()
        assert details[0]["dtype"] == np.int8, "Input must be INT8"

    def test_output_shape(self, interpreter):
        details = interpreter.get_output_details()
        shape = details[0]["shape"]
        assert shape[1] == 2, "Output must have 2 classes [no_rat, rat]"

    def test_output_dtype_int8(self, interpreter):
        details = interpreter.get_output_details()
        assert details[0]["dtype"] == np.int8, "Output must be INT8"

    def test_inference_runs(self, interpreter):
        """Smoke test — inference completes without error."""
        input_details  = interpreter.get_input_details()
        output_details = interpreter.get_output_details()

        dummy = np.zeros((1, 96, 96, 1), dtype=np.int8)
        interpreter.set_tensor(input_details[0]["index"], dummy)
        interpreter.invoke()

        output = interpreter.get_tensor(output_details[0]["index"])
        assert output.shape == (1, 2)

    def test_output_scores_sum_to_one(self, interpreter):
        """Dequantized softmax outputs should sum to ~1.0."""
        input_details  = interpreter.get_input_details()
        output_details = interpreter.get_output_details()

        dummy = np.random.randint(-128, 127, (1, 96, 96, 1), dtype=np.int8)
        interpreter.set_tensor(input_details[0]["index"], dummy)
        interpreter.invoke()

        output = interpreter.get_tensor(output_details[0]["index"])[0]
        scale      = output_details[0]["quantization_parameters"]["scales"][0]
        zero_point = output_details[0]["quantization_parameters"]["zero_points"][0]
        scores = (output.astype(np.float32) - zero_point) * scale

        assert abs(scores.sum() - 1.0) < 0.05, "Softmax scores should sum to ~1.0"

    def test_model_size_fits_esp32(self):
        """Model file must be under 300KB to fit in ESP32-C3 flash."""
        size_kb = os.path.getsize(MODEL_PATH) / 1024
        assert size_kb < 300, f"Model is {size_kb:.1f}KB — too large for ESP32-C3"
