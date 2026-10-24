#pragma once

#define MOTOR1_ENABLE 25//for enabling motor 1
#define MOTOR1_DIR    26//for setting direction of motor 1

#define MOTOR2_ENABLE 27//for enabling motor 2
#define MOTOR2_DIR    14//for setting direction of motor 2

void motor_gpio_init(void);

void motor1_forward(void);
void motor1_reverse(void);
void motor1_stop(void);
