#include "motors.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "MOTOR_TEST";//for logging information about the motor test

void motor_gpio_init(void)//function to initialize the GPIO pins for motor control
{
    gpio_config_t io_conf = {
        .pin_bit_mask =
            (1ULL << MOTOR1_ENABLE) |//for enabling motor 1
            (1ULL << MOTOR1_DIR) |//for setting direction of motor 1
            (1ULL << MOTOR2_ENABLE) |//for enabling motor 2
            (1ULL << MOTOR2_DIR),//for setting direction of motor 2

        .mode = GPIO_MODE_OUTPUT,//set the GPIO pins as output
        .pull_up_en = GPIO_PULLUP_DISABLE,//disable pull-up resistors
        .pull_down_en = GPIO_PULLDOWN_DISABLE,//disable pull-down resistors
        .intr_type = GPIO_INTR_DISABLE//disable interrupts
    };

    gpio_config(&io_conf);//configure the GPIO pins with the specified settings

    // Start with motors stopped
    gpio_set_level(MOTOR1_ENABLE, 0);//set motor 1 enable pin to low (stop motor 1)
    gpio_set_level(MOTOR2_ENABLE, 0);//set motor 2 enable pin to low (stop motor 2)
}

// -----------------------------
// MOTOR 1
// -----------------------------

void motor1_forward(void)//function to set motor 1 to move forward
{
    gpio_set_level(MOTOR1_DIR, 0);
    gpio_set_level(MOTOR1_ENABLE, 1);
    gpio_set_level(MOTOR2_ENABLE, 0); // Ensure motor 2 is stopped when motor 1 is moving forward
    gpio_set_level(MOTOR2_DIR, 0); // Set motor 2 direction to forward (optional, based on your logic)
    ESP_LOGI(TAG, "Motor 1 forward");
}

void motor1_reverse(void)//function to set motor 1 to move in reverse
{
    gpio_set_level(MOTOR1_DIR, 1);
    gpio_set_level(MOTOR1_ENABLE, 1);
    gpio_set_level(MOTOR2_ENABLE, 0); // Ensure motor 2 is stopped when motor 1 is moving in reverse
gpio_set_level(MOTOR2_DIR, 1); // Set motor 2 direction to reverse (optional, based on your logic)
    ESP_LOGI(TAG, "Motor 1 reverse");
}

void motor1_stop(void)//function to stop motor 1
{
    gpio_set_level(MOTOR1_ENABLE, 0);

    ESP_LOGI(TAG, "Motor 1 stop");
}

// -----------------------------
// MOTOR 2
// -----------------------------

void motor2_forward(void)//function to set motor 2 to move forward
{
    gpio_set_level(MOTOR2_DIR, 0);
    gpio_set_level(MOTOR2_ENABLE, 1);
    gpio_set_level(MOTOR1_ENABLE, 0); // Ensure motor 1 is stopped when motor 2 is moving forward
    gpio_set_level(MOTOR1_DIR, 0); // Set motor 1 direction to
    ESP_LOGI(TAG, "Motor 2 forward");
}

void motor2_reverse(void)//function to set motor 2 to move in reverse
{
    gpio_set_level(MOTOR1_ENABLE, 0);//stop motor 1 before reversing motor 2
    gpio_set_level(MOTOR2_DIR, 1);
    gpio_set_level(MOTOR2_ENABLE, 1);

    ESP_LOGI(TAG, "Motor 2 reverse");
}

void motor2_stop(void)//function to stop motor 2
{
    gpio_set_level(MOTOR2_ENABLE, 0);

    ESP_LOGI(TAG, "Motor 2 stop");
}

// -----------------------------
// BOTH MOTORS
// -----------------------------

void both_forward(void)//function to set both motors to move forward
{
    gpio_set_level(MOTOR1_DIR, 0);
    gpio_set_level(MOTOR2_DIR, 0);

    gpio_set_level(MOTOR1_ENABLE, 1);
    gpio_set_level(MOTOR2_ENABLE, 1);

    ESP_LOGI(TAG, "Both motors forward");
}

void both_reverse(void)//function to set both motors to move in reverse
{
    gpio_set_level(MOTOR1_DIR, 1);
    gpio_set_level(MOTOR2_DIR, 1);

    gpio_set_level(MOTOR1_ENABLE, 1);
    gpio_set_level(MOTOR2_ENABLE, 1);

    ESP_LOGI(TAG, "Both motors reverse");
}

void both_stop(void)//function to stop both motors
{
    gpio_set_level(MOTOR1_ENABLE, 0);
    gpio_set_level(MOTOR2_ENABLE, 0);

    ESP_LOGI(TAG, "Both motors stop");
}

