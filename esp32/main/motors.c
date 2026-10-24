#include "motors.h"
#include "driver/gpio.h"

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

