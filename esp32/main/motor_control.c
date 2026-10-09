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

    ESP_LOGI(TAG, "Motor 1 forward");
}

static void motor1_reverse(void)//function to set motor 1 to move in reverse
{
    gpio_set_level(MOTOR1_DIR, 1);
    gpio_set_level(MOTOR1_ENABLE, 1);

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

    ESP_LOGI(TAG, "Motor 2 forward");
}

static void motor2_reverse(void)//function to set motor 2 to move in reverse
{
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
    ESP_LOGI(TAG, "Starting motor test");

    motor_gpio_init();

    vTaskDelay(pdMS_TO_TICKS(1000));//wait for 1 second before starting the motor test

    // Motor 1 forward
    motor1_forward();
    vTaskDelay(pdMS_TO_TICKS(2000));

    motor1_stop();
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Motor 1 reverse
    motor1_reverse();
    vTaskDelay(pdMS_TO_TICKS(2000));

    motor1_stop();
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Motor 2 forward
    motor2_forward();
    vTaskDelay(pdMS_TO_TICKS(2000));

    motor2_stop();
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Motor 2 reverse
    motor2_reverse();
    vTaskDelay(pdMS_TO_TICKS(2000));

    motor2_stop();
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Both forward
    both_forward();
    vTaskDelay(pdMS_TO_TICKS(2000));

    both_stop();
    vTaskDelay(pdMS_TO_TICKS(1000));

    // Both reverse
    both_reverse();
    vTaskDelay(pdMS_TO_TICKS(2000));

    both_stop();//stop both motors
    vTaskDelay(pdMS_TO_TICKS(1000));

    ESP_LOGI(TAG, "Motor test complete");

    while (1)//infinite loop to keep the program running 
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}