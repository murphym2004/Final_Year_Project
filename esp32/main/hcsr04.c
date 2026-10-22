#include "hcsr04.h"
#include <stdbool.h>
#include <stddef.h>
#include "driver/gpio.h"
#include "esp_rom_sys.h"
#include "esp_timer.h"

#define TRIG_PIN GPIO_NUM_18//blue is front wire
#define ECHO_PIN GPIO_NUM_19
#define leftTRIG_PIN GPIO_NUM_17//left is grey
#define leftECHO_PIN GPIO_NUM_5
#define rightTRIG_PIN GPIO_NUM_4 //right is purple wire
#define rightECHO_PIN GPIO_NUM_16

#define TIMEOUT_US 30000
static bool initialized;

esp_err_t hcsr04_init(void)
{
    initialized = false;
    gpio_config_t config = {
        .pin_bit_mask = (1ULL << TRIG_PIN) | (1ULL << leftTRIG_PIN) |
                        (1ULL << rightTRIG_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    esp_err_t err = gpio_config(&config);
    if (err != ESP_OK) return err;
    err = gpio_set_level(TRIG_PIN, 0);
    if (err != ESP_OK) return err;
    err = gpio_set_level(leftTRIG_PIN, 0);
    if (err != ESP_OK) return err;
    err = gpio_set_level(rightTRIG_PIN, 0);
    if (err != ESP_OK) return err;
    config.pin_bit_mask = (1ULL << ECHO_PIN) | (1ULL << leftECHO_PIN) |
                          (1ULL << rightECHO_PIN);
    config.mode = GPIO_MODE_INPUT;
    err = gpio_config(&config);
    initialized = (err == ESP_OK);
    return err;
}

esp_err_t hcsr04_read_cm(float *distance_cm)
{
    if (distance_cm == NULL) return ESP_ERR_INVALID_ARG;
    if (!initialized) return ESP_ERR_INVALID_STATE;
    if (gpio_get_level(ECHO_PIN)) return ESP_ERR_INVALID_STATE;

    gpio_set_level(TRIG_PIN, 0);
    esp_rom_delay_us(2);
    gpio_set_level(TRIG_PIN, 1);
    esp_rom_delay_us(10);
    gpio_set_level(TRIG_PIN, 0);

    int64_t deadline = esp_timer_get_time() + TIMEOUT_US;
    while (!gpio_get_level(ECHO_PIN)) {
        if (esp_timer_get_time() >= deadline) return ESP_ERR_TIMEOUT;
    }
    int64_t start = esp_timer_get_time();
    while (gpio_get_level(ECHO_PIN)) {
        if (esp_timer_get_time() - start >= TIMEOUT_US) return ESP_ERR_TIMEOUT;
    }
    // Round-trip sound travel time, converted to centimetres.
    *distance_cm = (esp_timer_get_time() - start) * 0.0343f / 2.0f;
    return ESP_OK;
}

esp_err_t hcsr04_read_left_cm(float *distance_cm)
{
    if (distance_cm == NULL) return ESP_ERR_INVALID_ARG;
    if (!initialized) return ESP_ERR_INVALID_STATE;
    if (gpio_get_level(leftECHO_PIN)) return ESP_ERR_INVALID_STATE;

    gpio_set_level(leftTRIG_PIN, 0);
    esp_rom_delay_us(2);
    gpio_set_level(leftTRIG_PIN, 1);
    esp_rom_delay_us(10);
    gpio_set_level(leftTRIG_PIN, 0);

    int64_t deadline = esp_timer_get_time() + TIMEOUT_US;
    while (!gpio_get_level(leftECHO_PIN)) {
        if (esp_timer_get_time() >= deadline) return ESP_ERR_TIMEOUT;
    }
    int64_t start = esp_timer_get_time();
    while (gpio_get_level(leftECHO_PIN)) {
        if (esp_timer_get_time() - start >= TIMEOUT_US) return ESP_ERR_TIMEOUT;
    }
    // Round-trip sound travel time, converted to centimetres.
    *distance_cm = (esp_timer_get_time() - start) * 0.0343f / 2.0f;
    return ESP_OK;
}

esp_err_t hcsr04_read_right_cm(float *distance_cm)
{
    if (distance_cm == NULL) return ESP_ERR_INVALID_ARG;
    if (!initialized) return ESP_ERR_INVALID_STATE;
    if (gpio_get_level(rightECHO_PIN)) return ESP_ERR_INVALID_STATE;

    gpio_set_level(rightTRIG_PIN, 0);
    esp_rom_delay_us(2);
    gpio_set_level(rightTRIG_PIN, 1);
    esp_rom_delay_us(10);
    gpio_set_level(rightTRIG_PIN, 0);

    int64_t deadline = esp_timer_get_time() + TIMEOUT_US;
    while (!gpio_get_level(rightECHO_PIN)) {
        if (esp_timer_get_time() >= deadline) return ESP_ERR_TIMEOUT;
    }
    int64_t start = esp_timer_get_time();
    while (gpio_get_level(rightECHO_PIN)) {
        if (esp_timer_get_time() - start >= TIMEOUT_US) return ESP_ERR_TIMEOUT;
    }
    // Round-trip sound travel time, converted to centimetres.
    *distance_cm = (esp_timer_get_time() - start) * 0.0343f / 2.0f;
    return ESP_OK;
}
