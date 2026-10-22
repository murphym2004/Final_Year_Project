#include "hcsr04.h"
#include <stdio.h>//for printf function

#include "freertos/FreeRTOS.h"//makes main a task 
#include "freertos/task.h"//makes main a task

#include "driver/gpio.h"//for gpio functions

#include "esp_log.h"//for logging information

// -----------------------------
// GPIO DEFINITIONS
// -----------------------------

#define MOTOR1_ENABLE 25//for enabling motor 1
#define MOTOR1_DIR    26//for setting direction of motor 1

#define MOTOR2_ENABLE 27//for enabling motor 2
#define MOTOR2_DIR    14//for setting direction of motor 2

static const char *TAG = "MOTOR_TEST";//for logging information about the motor test

// -----------------------------
// GPIO SETUP
// -----------------------------

static void motor_gpio_init(void)//function to initialize the GPIO pins for motor control
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

static void motor1_forward(void)//function to set motor 1 to move forward
{
    gpio_set_level(MOTOR1_DIR, 0);
    gpio_set_level(MOTOR1_ENABLE, 1);
    gpio_set_level(MOTOR2_ENABLE, 0); // Ensure motor 2 is stopped when motor 1 is moving forward
    gpio_set_level(MOTOR2_DIR, 0); // Set motor 2 direction to forward (optional, based on your logic)
    ESP_LOGI(TAG, "Motor 1 forward");
}

static void motor1_reverse(void)//function to set motor 1 to move in reverse
{
    gpio_set_level(MOTOR1_DIR, 1);
    gpio_set_level(MOTOR1_ENABLE, 1);
    gpio_set_level(MOTOR2_ENABLE, 0); // Ensure motor 2 is stopped when motor 1 is moving in reverse
gpio_set_level(MOTOR2_DIR, 1); // Set motor 2 direction to reverse (optional, based on your logic)
    ESP_LOGI(TAG, "Motor 1 reverse");
}

static void motor1_stop(void)//function to stop motor 1
{
    gpio_set_level(MOTOR1_ENABLE, 0);

    ESP_LOGI(TAG, "Motor 1 stop");
}

// -----------------------------
// MOTOR 2
// -----------------------------

static void motor2_forward(void)//function to set motor 2 to move forward
{
    gpio_set_level(MOTOR2_DIR, 0);
    gpio_set_level(MOTOR2_ENABLE, 1);
    gpio_set_level(MOTOR1_ENABLE, 0); // Ensure motor 1 is stopped when motor 2 is moving forward
    gpio_set_level(MOTOR1_DIR, 0); // Set motor 1 direction to
    ESP_LOGI(TAG, "Motor 2 forward");
}

static void motor2_reverse(void)//function to set motor 2 to move in reverse
{
    gpio_set_level(MOTOR1_ENABLE, 0);//stop motor 1 before reversing motor 2
    gpio_set_level(MOTOR2_DIR, 1);
    gpio_set_level(MOTOR2_ENABLE, 1);

    ESP_LOGI(TAG, "Motor 2 reverse");
}

static void motor2_stop(void)//function to stop motor 2
{
    gpio_set_level(MOTOR2_ENABLE, 0);

    ESP_LOGI(TAG, "Motor 2 stop");
}

// -----------------------------
// BOTH MOTORS
// -----------------------------

static void both_forward(void)//function to set both motors to move forward
{
    gpio_set_level(MOTOR1_DIR, 0);
    gpio_set_level(MOTOR2_DIR, 0);

    gpio_set_level(MOTOR1_ENABLE, 1);
    gpio_set_level(MOTOR2_ENABLE, 1);

    ESP_LOGI(TAG, "Both motors forward");
}

static void both_reverse(void)//function to set both motors to move in reverse
{
    gpio_set_level(MOTOR1_DIR, 1);
    gpio_set_level(MOTOR2_DIR, 1);

    gpio_set_level(MOTOR1_ENABLE, 1);
    gpio_set_level(MOTOR2_ENABLE, 1);

    ESP_LOGI(TAG, "Both motors reverse");
}

static void both_stop(void)//function to stop both motors
{
    gpio_set_level(MOTOR1_ENABLE, 0);
    gpio_set_level(MOTOR2_ENABLE, 0);

    ESP_LOGI(TAG, "Both motors stop");
}

// -----------------------------
// MAIN
// -----------------------------

void app_main(void)
{
    motor_gpio_init();//initialize the GPIO pins for motor control
    hcsr04_init();//initialize the HCSR04 ultrasonic sensor

    while (1)//infinite loop to keep the program running 
    {
        float distance_cm;
        esp_err_t err = hcsr04_read_cm(&distance_cm);
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "Distance: %.1f cm", distance_cm);
        } else {
            ESP_LOGW(TAG, "Distance reading failed: %s", esp_err_to_name(err));
        }

        vTaskDelay(pdMS_TO_TICKS(100));

        float left_distance_cm;
        esp_err_t left_err = hcsr04_read_left_cm(&left_distance_cm);
        if (left_err == ESP_OK) {
            ESP_LOGI(TAG, "Left Distance: %.1f cm", left_distance_cm);
        } else {
            ESP_LOGW(TAG, "Left Distance reading failed: %s", esp_err_to_name(left_err));
        }

        vTaskDelay(pdMS_TO_TICKS(100));

        float right_distance_cm;
        esp_err_t right_err = hcsr04_read_right_cm(&right_distance_cm);
        if (right_err == ESP_OK) {
            ESP_LOGI(TAG, "Right Distance: %.1f cm", right_distance_cm);
        } else {
            ESP_LOGW(TAG, "Right Distance reading failed: %s", esp_err_to_name(right_err));
        }

        if(left_distance_cm < 20.0) {
            motor1_reverse();
            ESP_LOGI(TAG, "Left obstacle detected! Reversing motor 1.");
        } else if(right_distance_cm < 20.0) {
            motor2_reverse();
            ESP_LOGI(TAG, "Right obstacle detected! Reversing motor 2.");
        } else if(distance_cm < 20.0) {
            both_stop();
            ESP_LOGI(TAG, "Front obstacle detected! Stopping both motors.");
        } else {
            both_forward();
        }
        // front obstacle detection logic (commented out for now)
        // if (distance_cm < 20.0) {
        //     both_stop();           
        //     ESP_LOGI(TAG, "Obstacle detected! Stopping motors.");
        // } else {
        //     both_forward();
        // }

        // if sensor detects right obstacle, turn left
        // if (distance_cm < 20.0) {
        //     motor1_reverse();
        //      ESP_LOGI(TAG, "Obstacle detected! Stopping motors.");
        //  } else {
        //     both_forward();
        // }
    
        // if sensor detects left obstacle, turn left
        // if (distance_cm < 20.0) {            
        //     motor2_reverse();
        //     ESP_LOGI(TAG, "Obstacle detected! Stopping motors.");
        //  } else {
        //     both_forward();
        // }
    }
}
