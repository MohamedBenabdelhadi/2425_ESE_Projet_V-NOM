#include "driver_GP2Y0A41SK0F.h"
#include "adc.h"
#include <stdio.h>

#define DEBUG 1 /**< Enable or disable debug logs */

#if DEBUG
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

/**
 * @brief Initialize the GP2Y0A41SK0F sensors.
 * @param htof Pointer to the GP2Y0A41SK0F handle structure.
 */
void GP2Y0A41SK0F_Init(h_GP2Y0A41SK0F_t *htof) {
    // Assign ADC handles
    htof->hadc1 = &hadc1; // For ToF1
    htof->hadc2 = &hadc2; // For ToF2

    // Initialize ADC1
    if (HAL_ADC_Init(htof->hadc1) != HAL_OK) {
        Error_Handler();
    }

    // Initialize ADC2
    if (HAL_ADC_Init(htof->hadc2) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief Start ADC conversion using interrupts.
 * @param htof Pointer to the GP2Y0A41SK0F handle structure.
 */
void GP2Y0A41SK0F_Start_Interrupt(h_GP2Y0A41SK0F_t *htof) {
    HAL_StatusTypeDef status;

    // Start ADC1 in interrupt mode
    status = HAL_ADC_Start_IT(htof->hadc1);
    if (status != HAL_OK) {
        printf("ADC1 Interrupt Start Error: %d\n", status);
        Error_Handler();
    }

    // Start ADC2 in interrupt mode
    status = HAL_ADC_Start_IT(htof->hadc2);
    if (status != HAL_OK) {
        printf("ADC2 Interrupt Start Error: %d\n", status);
        Error_Handler();
    }
}

/**
 * @brief Stop ADC conversion.
 * @param htof Pointer to the GP2Y0A41SK0F handle structure.
 */
void GP2Y0A41SK0F_Stop_Interrupt(h_GP2Y0A41SK0F_t *htof) {
    // Stop ADC1
    if (HAL_ADC_Stop(htof->hadc1) != HAL_OK) {
        Error_Handler();
    }

    // Stop ADC2
    if (HAL_ADC_Stop(htof->hadc2) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief Compute distances for ToF1 based on ADC readings.
 */
void GP2Y0A41SK0F_get_distance1(h_GP2Y0A41SK0F_t *htof) {
    const float a = 120;
    const float b = 0.03;

    float Vo1 = (GP2Y0A41SK0F_ADC_VCC * htof->adc_val_tof1) / GP2Y0A41SK0F_ADC_MAX_VALUE;
    if (Vo1 > b) {
        htof->distance_tof1 = (int)(a / (Vo1 - b));
    } else {
        htof->distance_tof1 = -1; // Invalid distance
    }
}

/**
 * @brief Compute distances for ToF2 based on ADC readings.
 */
void GP2Y0A41SK0F_get_distance2(h_GP2Y0A41SK0F_t *htof) {
    const float a = 120;
    const float b = 0.03;

    float Vo2 = (GP2Y0A41SK0F_ADC_VCC * htof->adc_val_tof2) / GP2Y0A41SK0F_ADC_MAX_VALUE;
    if (Vo2 > b) {
        htof->distance_tof2 = (int)(a / (Vo2 - b));
    } else {
        htof->distance_tof2 = -1; // Invalid distance
    }
}
