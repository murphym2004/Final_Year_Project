#pragma once
#include "esp_err.h"

// Configure all three sensors. Pin assignments are in hcsr04.c.
esp_err_t hcsr04_init(void);

// Output is written only on success.
esp_err_t hcsr04_read_cm(float *distance_cm);
esp_err_t hcsr04_read_left_cm(float *distance_cm);
esp_err_t hcsr04_read_right_cm(float *distance_cm);
