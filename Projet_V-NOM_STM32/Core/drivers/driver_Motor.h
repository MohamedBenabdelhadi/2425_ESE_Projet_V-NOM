#ifndef DRIVER_MOTOR_H
#define DRIVER_MOTOR_H

#include "stm32g4xx_hal.h"

#define TIM_CHANNEL_1N 0x00000004U
#define TIM_CHANNEL_2N 0x00000004U

#define MOTOR1_SPEED_INCREASE_RATE 1
#define MOTOR2_SPEED_INCREASE_RATE 1

// Motors pin attribution
#define MOTOR1_FWD_PIN       GPIO_PIN_8   // FWD (TIM1 CH1)
#define MOTOR1_FWD_PORT      GPIOA        // Port FWD
#define MOTOR1_REV_PIN       GPIO_PIN_13  // REV (TIM1 CH1N)
#define MOTOR1_REV_PORT      GPIOB        // Port REV

#define MOTOR2_FWD_PIN       GPIO_PIN_9   // FWD (TIM2 CH2)
#define MOTOR2_FWD_PORT      GPIOA        // Port FWD
#define MOTOR2_REV_PIN       GPIO_PIN_14  // REV (TIM2 CH2N)
#define MOTOR2_REV_PORT      GPIOB        // Port REV


// Modes de fonctionnement du moteur
typedef enum {
	STANDBY_MODE, // Stops a motor
	FORWARD_MODE,
	REVERSE_MODE,
	BRAKE_MODE
} MotorMode;

typedef struct {
	MotorMode mode_mot1; // Operating mode for motor 1
	MotorMode mode_mot2; // Operating mode for motor 2
	int speed1;          // Target speed for motor 1
	int speed2;          // Target speed for motor 2

	// Private variables for speed variation
	int current_speed1;      // Current speed for motor 1
	int current_speed2;      // Current speed for motor 2

	TIM_HandleTypeDef * htim; // Timer handle for PWM control
} h_Motor_t;


// Function prototypes
void Motor_Init(h_Motor_t *hMotors, TIM_HandleTypeDef *htim);
void Motor_SetMode(h_Motor_t *hMotors);
void Motor_SetSpeed_percent(h_Motor_t *hMotor, float percent1, float percent2);
void Motor_Stop(h_Motor_t *hMotor);
void Motor_UpdateSpeed(h_Motor_t *hMotor);

#endif // DRIVER_MOTOR_H
