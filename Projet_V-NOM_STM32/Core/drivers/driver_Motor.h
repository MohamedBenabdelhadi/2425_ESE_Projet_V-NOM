#ifndef DRIVER_H
#define DRIVER_H

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
    STANDBY_MODE, // Stop a motor
    FORWARD_MODE,
    REVERSE_MODE,
    BRAKE_MODE
} MotorMode;

typedef struct {
	MotorMode mode_mot1;
	MotorMode mode_mot2;
	int speed1;
	int speed2;

	// Private variables for speed variation
	int current_speed1;
	int current_speed2;

	TIM_HandleTypeDef * htim; // Timer used for PWM
} h_Motor_t;


// Déclaration des fonctions
void Motor_Init(void);
void Motor_SetMode(uint8_t motor, MotorMode mode);
void Motor_Speed(uint8_t motor);
void Motor_StandbyAll(void);
#endif // DRIVER_H
