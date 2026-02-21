#pragma once
#include <stdint.h>
#include <stddef.h>

// Initialize TFLite model — call once at boot
bool detector_init();

// Run inference on raw image buffer
// Returns confidence score [0.0 - 1.0] for rat detection
// Returns -1.0 on error
float detector_run(const uint8_t* image_data, size_t image_len);
