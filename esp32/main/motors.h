#pragma once

#define MOTOR1_ENABLE 25//for enabling motor 1
#define MOTOR1_DIR    26//for setting direction of motor 1

#define MOTOR2_ENABLE 27//for enabling motor 2
#define MOTOR2_DIR    14//for setting direction of motor 2

void motor_gpio_init(void);

void motor1_forward(void);
void motor1_reverse(void);
void motor1_stop(void);

void motor2_forward(void);
void motor2_reverse(void);
void motor2_stop(void);

void both_forward(void);
void both_reverse(void);
void both_stop(void);

// Swapped steering mapping: motor 1 is the left track; motor 2 is the right track.
// Turns pivot in place with the tracks running in opposite directions.
void turn_left(void);
void turn_right(void);

// Forward curves: outside track forward, inside track stopped.
void steer_left(void);
void steer_right(void);
