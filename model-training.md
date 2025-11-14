# Model Training

### Training Pipeline

```bash
# Prepare dataset
python scripts/training/prepare_dataset.py

# Train model
python scripts/training/train_model.py --epochs 50 --batch-size 32

# Convert to TensorFlow Lite
python scripts/training/convert_to_tflite.py --model-path models/rat_detector.h5

# Evaluate performance
python scripts/training/evaluate_model.py --model-path models/rat_detector.tflite
```

**Model Requirements:**

* Input: 96×96 RGB image
* Output: Confidence score (0-100%)
* Threshold: ≥70%
* Quantization: INT8 for ESP32 compatibility

Models stored in `data/models/` and deployed to `src/edge/tflite_models/`.
