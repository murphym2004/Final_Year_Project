#include "hcsr04.h"
#include "motors.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdbool.h>

static const char *TAG = "ROBOT";

#define FRONT_STOP_CM 35.0f
#define FRONT_CLEAR_CM 45.0f
#define SIDE_STOP_CM 40.0f
#define SIDE_CLEAR_CM 48.0f
#define SENSOR_MAX_AGE_US 1000000LL
#define MIN_STEERING_US 500000LL

enum { FRONT, LEFT, RIGHT };
enum { STRAIGHT, STEER_LEFT, STEER_RIGHT };

void app_main(void)
{
    motor_gpio_init();
    ESP_ERROR_CHECK(hcsr04_init());

    // Each sensor keeps its last good distance and measurement time.
    float distance[3] = {0};
    int64_t last_good[3] = {0};
    const char *sensor_name[3] = {"Front", "Left", "Right"};
    int sensor = FRONT;
    int steering = STRAIGHT;
    bool front_escape = false;
    int64_t turn_started = 0;

    while (1) {
        // Read one sensor, then make a movement decision immediately.
        float reading = 0;
        esp_err_t error;
        if (sensor == FRONT) error = hcsr04_read_cm(&reading);
        else if (sensor == LEFT) error = hcsr04_read_left_cm(&reading);
        else error = hcsr04_read_right_cm(&reading);

        int64_t now = esp_timer_get_time();
        if (error == ESP_OK) {
            distance[sensor] = reading;
            last_good[sensor] = now;
            ESP_LOGI(TAG, "%s: %.1f cm", sensor_name[sensor], reading);
        } else {
            ESP_LOGW(TAG, "%s: %s", sensor_name[sensor], esp_err_to_name(error));
        }
        sensor = (sensor + 1) % 3;

        // Brief failures reuse the last good reading; old readings stop us.
        if (last_good[FRONT] == 0 || last_good[LEFT] == 0 || last_good[RIGHT] == 0 ||
            now - last_good[FRONT] > SENSOR_MAX_AGE_US ||
            now - last_good[LEFT] > SENSOR_MAX_AGE_US ||
            now - last_good[RIGHT] > SENSOR_MAX_AGE_US) {
            both_stop();
            ESP_LOGW(TAG, "Stopped: waiting for recent sensor readings");
        } else if (distance[FRONT] < FRONT_STOP_CM) {
            // Front obstacle: back away, then pivot toward the clearer side.
            steering = distance[LEFT] >= distance[RIGHT] ? STEER_LEFT : STEER_RIGHT;
            front_escape = true;
            ESP_LOGI(TAG, "Front escape: pivot %s",
                     steering == STEER_LEFT ? "left" : "right");
            both_stop();
            vTaskDelay(pdMS_TO_TICKS(50));
            both_reverse();
            vTaskDelay(pdMS_TO_TICKS(500));
            both_stop();
            vTaskDelay(pdMS_TO_TICKS(50));
            if (steering == STEER_LEFT) turn_left();
            else turn_right();
            vTaskDelay(pdMS_TO_TICKS(600));
            both_stop();

            // The robot has moved: collect new readings before driving.
            last_good[FRONT] = last_good[LEFT] = last_good[RIGHT] = 0;
            sensor = FRONT;
        } else {
            // Finish a turn only when there is enough clearance.
            if (steering != STRAIGHT) {
                int wall_side = steering == STEER_LEFT ? RIGHT : LEFT;
                bool ready = now - turn_started >= MIN_STEERING_US;
                if (front_escape) ready = distance[FRONT] >= FRONT_CLEAR_CM;
                if (distance[wall_side] >= SIDE_CLEAR_CM && ready) {
                    steering = STRAIGHT;
                    front_escape = false;
                }
            }

            // Choose a new side-steering direction when travelling straight.
            if (steering == STRAIGHT) {
                if (distance[LEFT] < SIDE_STOP_CM && distance[RIGHT] < SIDE_STOP_CM)
                    steering = distance[LEFT] >= distance[RIGHT] ? STEER_LEFT : STEER_RIGHT;
                else if (distance[RIGHT] < SIDE_STOP_CM) steering = STEER_LEFT;
                else if (distance[LEFT] < SIDE_STOP_CM) steering = STEER_RIGHT;
                if (steering != STRAIGHT) turn_started = now;
            }

            // Front escapes pivot; side avoidance curves forward.
            if (steering == STEER_LEFT) {
                if (front_escape) turn_left();
                else steer_left();
            } else if (steering == STEER_RIGHT) {
                if (front_escape) turn_right();
                else steer_right();
            } else {
                both_forward();
            }
        }
        // Keep ultrasonic triggers apart to reduce interference.
        vTaskDelay(pdMS_TO_TICKS(60));
    }
}
