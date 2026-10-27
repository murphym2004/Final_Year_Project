#include "motors.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "MOTOR_TEST";//for logging information about the motor test
static int track1_direction = -1;
static int track2_direction = -1;

// PWM power percentages; these are not measured wheel speeds.
#define DRIVE_POWER_PERCENT 100
#define CRUISE_POWER_PERCENT 85
#define STEER_OUTER_POWER_PERCENT 100
// A stopped inner track gives a sharper forward arc without reversing it.
#define STEER_INNER_POWER_PERCENT 0

static void motor_power(ledc_channel_t channel, unsigned percent)
{
    if (percent > 100) percent = 100;
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, channel,
                                 percent * 256 / 100));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, channel));
}

static void drive_tracks(int motor1_reverse, int motor2_reverse,
                         unsigned motor1_power, unsigned motor2_power)
{
    // Remove drive power before changing either track's direction.
    if (track1_direction != motor1_reverse ||
        track2_direction != motor2_reverse) {
        motor_power(LEDC_CHANNEL_0, 0);
        motor_power(LEDC_CHANNEL_1, 0);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
    gpio_set_level(MOTOR1_DIR, motor1_reverse);
    gpio_set_level(MOTOR2_DIR, motor2_reverse);
    track1_direction = motor1_reverse;
    track2_direction = motor2_reverse;
    motor_power(LEDC_CHANNEL_0, motor1_power);
    motor_power(LEDC_CHANNEL_1, motor2_power);
}

void motor_gpio_init(void)//function to initialize the GPIO pins for motor control
{
    track1_direction = track2_direction = -1;
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

    // Route PWM to the existing enable pins, starting with both motors off.
    ledc_timer_config_t timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 1000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer));
    ledc_channel_config_t channel = {
        .gpio_num = MOTOR1_ENABLE,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&channel));
    channel.gpio_num = MOTOR2_ENABLE;
    channel.channel = LEDC_CHANNEL_1;
    ESP_ERROR_CHECK(ledc_channel_config(&channel));
}

void turn_left(void)
{
    // Swapped motor mapping: motor 1 left reverse, motor 2 right forward.
    drive_tracks(1, 0, DRIVE_POWER_PERCENT, DRIVE_POWER_PERCENT);
}

void turn_right(void)
{
    drive_tracks(0, 1, DRIVE_POWER_PERCENT, DRIVE_POWER_PERCENT);
}

void steer_left(void)
{
    drive_tracks(0, 0, STEER_INNER_POWER_PERCENT, STEER_OUTER_POWER_PERCENT);
    ESP_LOGI(TAG, "Side steer LEFT: motor 1/left off, motor 2/right forward %u%%",
             (unsigned)STEER_OUTER_POWER_PERCENT);
}

void steer_right(void)
{
    drive_tracks(0, 0, STEER_OUTER_POWER_PERCENT, STEER_INNER_POWER_PERCENT);
    ESP_LOGI(TAG, "Side steer RIGHT: motor 1/left forward %u%%, motor 2/right off",
             (unsigned)STEER_OUTER_POWER_PERCENT);
}

// -----------------------------
// MOTOR 1
// -----------------------------

void motor1_forward(void)//function to set motor 1 to move forward
{
    drive_tracks(0, 0, DRIVE_POWER_PERCENT, 0);
    ESP_LOGI(TAG, "Motor 1 forward");
}

void motor1_reverse(void)//function to set motor 1 to move in reverse
{
    drive_tracks(1, 1, DRIVE_POWER_PERCENT, 0);
    ESP_LOGI(TAG, "Motor 1 reverse");
}

void motor1_stop(void)//function to stop motor 1
{
    motor_power(LEDC_CHANNEL_0, 0);

    ESP_LOGI(TAG, "Motor 1 stop");
}

// -----------------------------
// MOTOR 2
// -----------------------------

void motor2_forward(void)//function to set motor 2 to move forward
{
    drive_tracks(0, 0, 0, DRIVE_POWER_PERCENT);
    ESP_LOGI(TAG, "Motor 2 forward");
}

void motor2_reverse(void)//function to set motor 2 to move in reverse
{
    drive_tracks(0, 1, 0, DRIVE_POWER_PERCENT);

    ESP_LOGI(TAG, "Motor 2 reverse");
}

void motor2_stop(void)//function to stop motor 2
{
    motor_power(LEDC_CHANNEL_1, 0);

    ESP_LOGI(TAG, "Motor 2 stop");
}

// -----------------------------
// BOTH MOTORS
// -----------------------------

void both_forward(void)//function to set both motors to move forward
{
    drive_tracks(0, 0, CRUISE_POWER_PERCENT, CRUISE_POWER_PERCENT);

    ESP_LOGI(TAG, "Both motors forward");
}

void both_reverse(void)//function to set both motors to move in reverse
{
    drive_tracks(1, 1, DRIVE_POWER_PERCENT, DRIVE_POWER_PERCENT);

    ESP_LOGI(TAG, "Both motors reverse");
}

void both_stop(void)//function to stop both motors
{
    motor_power(LEDC_CHANNEL_0, 0);
    motor_power(LEDC_CHANNEL_1, 0);

    ESP_LOGI(TAG, "Both motors stop");
}

