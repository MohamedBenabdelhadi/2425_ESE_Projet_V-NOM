#include "driver_Motor.h"
#include "tim.h"  // Inclusion pour l'accès aux timers

#define TIM_CHANNEL_1N 0x00000004U
#define TIM_CHANNEL_2N 0x00000004U

#define PWM_MAX 4000  // Valeur maximale pour le signal PWM


#define MOTOR1_FWD_PIN       GPIO_PIN_8   // FWD (TIM1 CH1)
#define MOTOR1_FWD_PORT      GPIOA        // Port FWD
#define MOTOR1_REV_PIN       GPIO_PIN_13  // REV (TIM1 CH1N)
#define MOTOR1_REV_PORT      GPIOB        // Port REV


#define MOTOR2_FWD_PIN       GPIO_PIN_9   // FWD (TIM2 CH2)
#define MOTOR2_FWD_PORT      GPIOA        // Port FWD
#define MOTOR2_REV_PIN       GPIO_PIN_14  // REV (TIM2 CH2N)
#define MOTOR2_REV_PORT      GPIOB        // Port REV

// Initialisation des moteurs
void Motor_Init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    MX_TIM1_Init();
    MX_TIM2_Init();


    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);

    Motor_SetMode(1, STANDBY_MODE);
    Motor_SetMode(2, STANDBY_MODE);
    //Motor_Speed(1, 0);
    //Motor_Spee(2, 0);
}

// Définir le mode de fonctionnement d'un moteur
void Motor_SetMode(uint8_t motor, MotorMode mode) {
    if (motor == 1) {
        switch (mode) {
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
    } else if (motor == 2) {
        switch (mode) {
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
}

// Augmenter et diminuer la vitesse progressivement
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

// Arrêter tous les moteurs
void Motor_Stop(void) {
    Motor_SetMode(1, STANDBY_MODE);
    Motor_SetMode(2, STANDBY_MODE);

}
