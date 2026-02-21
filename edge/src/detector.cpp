#include "detector.h"
#include "config.h"
#include <Arduino.h>

// TFLite Micro includes
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"

// Model data — generated from guarden_v1_int8.tflite via xxd
// Place model_data.h in this directory after training
// xxd -i guarden_v1_int8.tflite > model_data.h
#include "model_data.h"

// ─── TFLite globals ───────────────────────────────────────────────────────────
namespace {
    // Tensor arena — tune this to fit in 400KB RAM
    constexpr int kTensorArenaSize = 80 * 1024; // 80KB
    uint8_t tensor_arena[kTensorArenaSize];

    const tflite::Model* model = nullptr;
    tflite::MicroInterpreter* interpreter = nullptr;
    TfLiteTensor* input = nullptr;
    TfLiteTensor* output = nullptr;
}

bool detector_init() {
    model = tflite::GetModel(g_model_data); // from model_data.h
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        Serial.println("[DET] Model schema mismatch");
        return false;
    }

    static tflite::MicroMutableOpResolver<5> resolver;
    resolver.AddConv2D();
    resolver.AddDepthwiseConv2D();
    resolver.AddReshape();
    resolver.AddSoftmax();
    resolver.AddAveragePool2D();

    static tflite::MicroInterpreter static_interpreter(
        model, resolver, tensor_arena, kTensorArenaSize
    );
    interpreter = &static_interpreter;

    if (interpreter->AllocateTensors() != kTfLiteOk) {
        Serial.println("[DET] AllocateTensors failed");
        return false;
    }

    input  = interpreter->input(0);
    output = interpreter->output(0);

    Serial.printf("[DET] Init OK | Arena used: %d bytes\n",
                  interpreter->arena_used_bytes());
    return true;
}

float detector_run(const uint8_t* image_data, size_t image_len) {
    if (!interpreter) return -1.0f;

    // Copy image into input tensor
    // Model expects INT8 input normalized to [-128, 127]
    int8_t* input_buf = input->data.int8;
    for (size_t i = 0; i < image_len && i < (size_t)(MODEL_INPUT_W * MODEL_INPUT_H); i++) {
        input_buf[i] = (int8_t)(image_data[i] - 128);
    }

    // Run inference
    if (interpreter->Invoke() != kTfLiteOk) {
        Serial.println("[DET] Invoke failed");
        return -1.0f;
    }

    // Output: [no_rat, rat] — index 1 is rat confidence
    // Dequantize INT8 output to float
    float scale      = output->params.scale;
    int   zero_point = output->params.zero_point;
    float rat_score  = (output->data.int8[1] - zero_point) * scale;

    return rat_score;
}
