/**
 * @file motor_control.c
 * @brief Motor control implementation for initializing, setting modes, controlling speed, and updating motor states.
 */

#include "driver_Motor.h"
#include "tim.h"  // Inclusion for access to timers
#include <stdio.h>

#define DEBUG 0 /**< Enable or disable debug logs */

#if DEBUG
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif


/**
 * @brief Initialize the motors.
 * @param hMotors Pointer to the motor handle structure.
 * @param htim Pointer to the timer handle used for PWM control.
 */
void Motor_Init(h_Motor_t * hMotors, TIM_HandleTypeDef * htim) {
	// Allows to get direct parameters from the timer used
	hMotors->htim = htim;
	hMotors->speed_increase_rate1 = 10;
	hMotors->speed_increase_rate2 = 10;

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	MX_TIM1_Init();
	//MX_TIM2_Init();

	HAL_TIM_PWM_Start(hMotors->htim, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(hMotors->htim, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(hMotors->htim, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(hMotors->htim, TIM_CHANNEL_2);

	hMotors->mode_mot1 = STANDBY_MODE;
	hMotors->mode_mot2 = STANDBY_MODE;
	Motor_SetMode(hMotors);
}

/**
 * @brief Set the operating mode of the motors.
 * @param hMotors Pointer to the motor handle structure.
 */
void Motor_SetMode(h_Motor_t * hMotors)
{
	switch (hMotors->mode_mot1) {
	case FORWARD_MODE:
		DEBUG_PRINT("Mot1: FORWARD_MODE\r\n");
		HAL_TIM_PWM_Start(hMotors->htim, TIM_CHANNEL_1);
		HAL_TIMEx_PWMN_Stop(hMotors->htim, TIM_CHANNEL_1);
		break;

	case REVERSE_MODE:
		DEBUG_PRINT("Mot1: REVERSE_MODE\r\n");
		HAL_TIMEx_PWMN_Start(hMotors->htim, TIM_CHANNEL_1);
		HAL_TIM_PWM_Stop(hMotors->htim, TIM_CHANNEL_1);
		break;

	case STANDBY_MODE:
	default:
		HAL_TIMEx_PWMN_Stop(hMotors->htim, TIM_CHANNEL_1);
		HAL_TIM_PWM_Stop(hMotors->htim, TIM_CHANNEL_1);
		__HAL_TIM_SET_COMPARE(hMotors->htim, TIM_CHANNEL_1, 0);
		break;
	}

	switch (hMotors->mode_mot2) {
	case FORWARD_MODE:
		DEBUG_PRINT("Mot2: FORWARD_MODE\r\n");
		HAL_TIM_PWM_Start(hMotors->htim, TIM_CHANNEL_2);
		HAL_TIMEx_PWMN_Stop(hMotors->htim, TIM_CHANNEL_2);
		break;

	case REVERSE_MODE:
		DEBUG_PRINT("Mot2: REVERSE_MODE\r\n");
		HAL_TIMEx_PWMN_Start(hMotors->htim, TIM_CHANNEL_2);
		HAL_TIM_PWM_Stop(hMotors->htim, TIM_CHANNEL_2);
		break;

	case STANDBY_MODE:
	default:
		HAL_TIMEx_PWMN_Stop(hMotors->htim, TIM_CHANNEL_2);
		HAL_TIM_PWM_Stop(hMotors->htim, TIM_CHANNEL_2);
		__HAL_TIM_SET_COMPARE(hMotors->htim, TIM_CHANNEL_2, 0);
		break;
	}
}

/**
 * @brief Set the speed of the motors as a percentage of maximum speed.
 * @param hMotor Pointer to the motor handle structure.
 * @param percent1 Speed percentage for motor 1 (0.0 to 100.0).
 * @param percent2 Speed percentage for motor 2 (0.0 to 100.0).
 */
void Motor_SetSpeed_percent(h_Motor_t * hMotor, float percent1, float percent2) {
	hMotor->speed1 = (int)(MOTOR1_REV_DIFF * hMotor->htim->Init.Period * percent1/100.0);
	hMotor->speed2 = (int)(hMotor->htim->Init.Period * percent2/100.0);
}

/**
 * @brief Stop all motors by setting their mode to standby.
 * @param hMotor Pointer to the motor handle structure.
 */
void Motor_Stop(h_Motor_t * hMotors) {
	hMotors->mode_mot1 = STANDBY_MODE;
	hMotors->mode_mot2 = STANDBY_MODE;
	Motor_SetMode(hMotors);
}

/**
 * @brief Update motor speeds gradually to achieve smoother transitions.
 * @param hMotor Pointer to the motor handle structure.
 */
void Motor_UpdateSpeed(h_Motor_t * hMotors) {
	// Increase speed progressively
	if (hMotors->speed1 > hMotors->current_speed1)
		hMotors->current_speed1 += hMotors->speed_increase_rate1;
	if (hMotors->speed2 > hMotors->current_speed2)
		hMotors->current_speed2 += hMotors->speed_increase_rate2;

	// Decrease speed progressively
	if (hMotors->speed1 < hMotors->current_speed1)
		hMotors->current_speed1 -= hMotors->speed_increase_rate1;
	if (hMotors->speed2 < hMotors->current_speed2)
		hMotors->current_speed2 -= hMotors->speed_increase_rate2;

	__HAL_TIM_SET_COMPARE(hMotors->htim, TIM_CHANNEL_1, hMotors->current_speed1);
	__HAL_TIM_SET_COMPARE(hMotors->htim, TIM_CHANNEL_2, hMotors->current_speed2);
}
