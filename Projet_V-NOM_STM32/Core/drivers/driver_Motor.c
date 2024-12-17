#include "driver_Motor.h"
#include "tim.h"  // Inclusion pour l'accès aux timers


typedef struct {
	MotorMode mode_mot1;
	MotorMode mode_mot2;
	int speed1;
	int speed2;
} h_Motor_t;


/*
 * Initialisation des moteurs
 */
void Motor_Init(h_Motor_t * hMotors) {
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	MX_TIM1_Init();
	MX_TIM2_Init();

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);

	hMotors->mode_mot1 = STANDBY_MODE;
	hMotors->mode_mot2 = STANDBY_MODE;
	Motor_SetMode(hMotors);

	//Motor_Speed(1, 0);
	//Motor_Spee(2, 0);
}

/*
 * Définir le mode de fonctionnement d'un moteur
 */
void Motor_SetMode(h_Motor_t * hMotors)
{
	switch (hMotors->mode_mot1)
	{
	case STANDBY_MODE:
		HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
		break;

	case FORWARD_MODE:
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
		HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
		break;

	case REVERSE_MODE:
		HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
		break;

	case BRAKE_MODE:
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
		HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, PWM_MAX);
		break;
	}

	switch (hMotors->mode_mot2)
	{
	case STANDBY_MODE:
		HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
		break;

	case FORWARD_MODE:
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
		HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
		break;

	case REVERSE_MODE:
		HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
		break;

	case BRAKE_MODE:
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
		HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, PWM_MAX);
		break;
	}
}

/*
 * Augmenter et diminuer la vitesse progressivement
 */
void Motor_Speed(uint8_t motor) {

	for (int i = 0; i <= 4000; i++) {
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, i);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, i);
		HAL_Delay(1);
	}

	for (int i = 3999; i >= 0; i--) {
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, i);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, i);
		HAL_Delay(1);;
	}
}

/*
 * Arrêter tous les moteurs
 */
void Motor_Stop(void) {
	hMotors->mode_mot1 = STANDBY_MODE;
	hMotors->mode_mot2 = STANDBY_MODE;
	Motor_SetMode(hMotors);
}
