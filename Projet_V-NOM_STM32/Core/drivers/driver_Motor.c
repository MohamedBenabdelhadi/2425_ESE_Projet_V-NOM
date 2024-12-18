/**
 * @file motor_control.c
 * @brief Motor control implementation for initializing, setting modes, controlling speed, and updating motor states.
 */

#include "driver_Motor.h"
#include "tim.h"  // Inclusion for access to timers

/**
 * @brief Initialize the motors.
 * @param hMotors Pointer to the motor handle structure.
 * @param htim Pointer to the timer handle used for PWM control.
 */
void Motor_Init(h_Motor_t * hMotors, TIM_HandleTypeDef * htim) {
    // Allows to get direct parameters from the timer used
    hMotors->htim = htim;

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    MX_TIM1_Init();
    MX_TIM2_Init();

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
void Motor_SetMode(h_Motor_t * hMotors) {
    switch (hMotors->mode_mot1) {
        case FORWARD_MODE:
            HAL_TIM_PWM_Start(hMotors->htim, TIM_CHANNEL_1);
            HAL_TIMEx_PWMN_Stop(hMotors->htim, TIM_CHANNEL_1);
            break;

        case REVERSE_MODE:
            HAL_TIMEx_PWMN_Start(hMotors->htim, TIM_CHANNEL_1);
            HAL_TIM_PWM_Stop(hMotors->htim, TIM_CHANNEL_1);
            break;

        case BRAKE_MODE:
            HAL_TIM_PWM_Start(hMotors->htim, TIM_CHANNEL_1);
            HAL_TIMEx_PWMN_Start(hMotors->htim, TIM_CHANNEL_1);
            __HAL_TIM_SET_COMPARE(hMotors->htim, TIM_CHANNEL_1, hMotors->htim->Init->Period);
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
            HAL_TIM_PWM_Start(hMotors->htim, TIM_CHANNEL_2);
            HAL_TIMEx_PWMN_Stop(hMotors->htim, TIM_CHANNEL_2);
            break;

        case REVERSE_MODE:
            HAL_TIMEx_PWMN_Start(hMotors->htim, TIM_CHANNEL_2);
            HAL_TIM_PWM_Stop(hMotors->htim, TIM_CHANNEL_2);
            break;

        case BRAKE_MODE:
            HAL_TIM_PWM_Start(hMotors->htim, TIM_CHANNEL_2);
            HAL_TIMEx_PWMN_Start(hMotors->htim, TIM_CHANNEL_2);
            __HAL_TIM_SET_COMPARE(hMotors->htim, TIM_CHANNEL_2, hMotors->htim->Init->Period);
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
    hMotor->speed1 = (int)(hMotor->htim->Init->Period * percent1/100.0);
    hMotor->speed2 = (int)(hMotor->htim->Init->Period * percent2/100.0);
}

/**
 * @brief Stop all motors by setting their mode to standby.
 * @param hMotor Pointer to the motor handle structure.
 */
void Motor_Stop(h_Motor_t * hMotor) {
    hMotor->mode_mot1 = STANDBY_MODE;
    hMotor->mode_mot2 = STANDBY_MODE;
    Motor_SetMode(hMotor);
}

/**
 * @brief Update motor speeds gradually to achieve smoother transitions.
 * @param hMotor Pointer to the motor handle structure.
 */
void Motor_UpdateSpeed(h_Motor_t * hMotor) {
    // Increase speed progressively
    if (hMotor->speed1 > hMotor->current_speed1)
        hMotor->current_speed1 += MOTOR1_SPEED_INCREASE_RATE;
    if (hMotor->speed2 > hMotor->current_speed2)
        hMotor->current_speed2 += MOTOR2_SPEED_INCREASE_RATE;

    // Decrease speed progressively
    if (hMotor->speed1 < hMotor->current_speed1)
        hMotor->current_speed1 -= MOTOR1_SPEED_INCREASE_RATE;
    if (hMotor->speed2 < hMotor->current_speed2)
        hMotor->current_speed2 -= MOTOR2_SPEED_INCREASE_RATE;

    __HAL_TIM_SET_COMPARE(hMotor->htim, TIM_CHANNEL_1, hMotor->current_speed1);
    __HAL_TIM_SET_COMPARE(hMotor->htim, TIM_CHANNEL_2, hMotor->current_speed2);
}
