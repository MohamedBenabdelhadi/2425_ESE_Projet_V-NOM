/*
 * driver_GP2Y0A41SK0F.h
 *
 *  Created on: Oct 22, 2024
 *      Author: oliver
 */

#include <stdlib.h>
#include <inttypes.h>
#include "adc.h"

#define GP2Y0A41SK0F_ADC_VCC 5.0
#define GP2Y0A41SK0F_ADC_BITS 12 ///< Number of ADC bits used.
#define GP2Y0A41SK0F_ADC_MAX_VALUE ((1 << GP2Y0A41SK0F_ADC_BITS) - 1) ///< Maximum ADC value.


/**
 * @brief Structure to handle GP2Y0A41SK0F sensor data and configurations.
 */
typedef struct {
    ADC_HandleTypeDef *hadc; ///< ADC handle used for measurements.
    ADC_ChannelConfTypeDef cConfig_tof1; ///< ADC channel configuration for ToF sensor 1.
    ADC_ChannelConfTypeDef cConfig_tof2; ///< ADC channel configuration for ToF sensor 2.

    uint32_t adc_val_tof1; ///< Raw ADC value from ToF sensor 1.
    uint32_t adc_val_tof2; ///< Raw ADC value from ToF sensor 2.

    int distance_tof1; ///< Computed distance from ToF sensor 1 in millimeters.
    int distance_tof2; ///< Computed distance from ToF sensor 2 in millimeters.
} h_GP2Y0A41SK0F_t;


/**
 * @brief Initialize the GP2Y0A41SK0F sensors.
 * @param htof Pointer to the GP2Y0A41SK0F handle structure.
 */
void GP2Y0A41SK0F_Init(h_GP2Y0A41SK0F_t *htof);

/**
 * @brief Read raw ADC value from ToF sensor 1.
 * @param htof Pointer to the GP2Y0A41SK0F handle structure.
 */
void GP2Y0A41SK0F_Read_ToF1(h_GP2Y0A41SK0F_t *htof);

/**
 * @brief Read raw ADC value from ToF sensor 2.
 * @param htof Pointer to the GP2Y0A41SK0F handle structure.
 */
void GP2Y0A41SK0F_Read_ToF2(h_GP2Y0A41SK0F_t *htof);

/**
 * @brief Compute distances for both ToF sensors based on ADC readings.
 * @param htof Pointer to the GP2Y0A41SK0F handle structure.
 */
void GP2Y0A41SK0F_get_distance(h_GP2Y0A41SK0F_t *htof);
