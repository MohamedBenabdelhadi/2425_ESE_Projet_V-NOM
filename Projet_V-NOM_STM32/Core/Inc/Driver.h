#ifndef DRIVER_H
#define DRIVER_H

#include "stm32g4xx_hal.h"

// Modes de fonctionnement du moteur
typedef enum {
    STANDBY_MODE,
    FORWARD_MODE,
    REVERSE_MODE,
    BRAKE_MODE
} MotorMode;

// Déclaration des fonctions
void Motor_Init(void);
void Motor_SetMode(uint8_t motor, MotorMode mode);
void Motor_Speed(uint8_t motor);
void Motor_StandbyAll(void);
#endif // DRIVER_H
