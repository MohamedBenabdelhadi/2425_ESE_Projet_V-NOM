#ifndef DRIVER_GP2Y0A41SK0F_H
#define DRIVER_GP2Y0A41SK0F_H

#include <stdlib.h>
#include <inttypes.h>
#include "adc.h"

#define GP2Y0A41SK0F_ADC_VCC 5.0
#define GP2Y0A41SK0F_ADC_BITS 12 ///< Number of ADC bits used.
#define GP2Y0A41SK0F_ADC_MAX_VALUE ((1 << GP2Y0A41SK0F_ADC_BITS) - 1) ///< Maximum ADC value.

typedef struct {
    ADC_HandleTypeDef *hadc1; ///< ADC1 handle for ToF1.
    ADC_HandleTypeDef *hadc2; ///< ADC2 handle for ToF2.

    uint32_t adc_val_tof1; ///< Raw ADC value from ToF1.
    uint32_t adc_val_tof2; ///< Raw ADC value from ToF2.

    int distance_tof1; ///< Computed distance from ToF1 in millimeters.
    int distance_tof2; ///< Computed distance from ToF2 in millimeters.
} h_GP2Y0A41SK0F_t;

void GP2Y0A41SK0F_Init(h_GP2Y0A41SK0F_t *htof);
void GP2Y0A41SK0F_Start_Interrupt(h_GP2Y0A41SK0F_t *htof);
void GP2Y0A41SK0F_Stop_Interrupt(h_GP2Y0A41SK0F_t *htof);
void GP2Y0A41SK0F_get_distance1(h_GP2Y0A41SK0F_t *htof);
void GP2Y0A41SK0F_get_distance2(h_GP2Y0A41SK0F_t *htof);

#endif // DRIVER_GP2Y0A41SK0F_H
