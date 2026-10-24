#include "hcsr04.h"
#include "motors.h"
#include <stdio.h>//for printf function

#include "freertos/FreeRTOS.h"//makes main a task 
#include "freertos/task.h"//makes main a task

#include "driver/gpio.h"//for gpio functions

#include "esp_log.h"//for logging information

static const char *TAG = "MOTOR_TEST";//for logging information about the motor test

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
