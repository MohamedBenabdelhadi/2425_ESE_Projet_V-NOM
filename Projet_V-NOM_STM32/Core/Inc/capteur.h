#ifndef CAPTEUR_H
#define CAPTEUR_H

#include <stdint.h>
#include "stm32g4xx_hal.h"

void Capteur_Init(void);
uint32_t Capteur_Read_ToF1(void);
uint32_t Capteur_Read_ToF2(void);
#endif

