#pragma once
#include "esp_err.h"

// Trig: GPIO 18. Echo: GPIO 19 (use a voltage divider).
esp_err_t hcsr04_init(void);
// Use from one task; allow at least 60 ms between readings.
// Output is written only on success.
esp_err_t hcsr04_read_cm(float *distance_cm);
