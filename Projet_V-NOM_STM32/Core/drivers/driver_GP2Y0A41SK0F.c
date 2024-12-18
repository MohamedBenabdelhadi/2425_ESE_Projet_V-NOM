/*
 * driver_GP2Y0A41SK0F.c
 *
 *  Created on: Oct 22, 2024
 *      Author: oliver
 */

#include "driver_GP2Y0A41SK0F.h"
#include "adc.h"
#include "gpio.h"
#include <stdio.h>


/**
 * @brief Initialize the GP2Y0A41SK0F sensors.
 * @param htof Pointer to the GP2Y0A41SK0F handle structure.
 */
void GP2Y0A41SK0F_Init(h_GP2Y0A41SK0F_t * htof) {
	// The ADC used is wired on the board so it's ADC1
	htof->hadc = &hadc1;

	if (HAL_ADC_Init(htof->hadc) != HAL_OK) {
		Error_Handler();
	}

	// Channel Config ADC ToF1
	htof->cConfig_tof1.Channel = ADC_CHANNEL_3;
	htof->cConfig_tof1.Rank = ADC_REGULAR_RANK_1;
	htof->cConfig_tof1.SamplingTime = ADC_SAMPLETIME_47CYCLES_5;

	// Channel Config ADC ToF2
	htof->cConfig_tof2.Channel = ADC_CHANNEL_1;
	htof->cConfig_tof2.Rank = ADC_REGULAR_RANK_1;
	htof->cConfig_tof2.SamplingTime = ADC_SAMPLETIME_47CYCLES_5;
}

/**
 * @brief Read raw ADC value from ToF sensor 1.
 * @param htof Pointer to the GP2Y0A41SK0F handle structure.
 */
void GP2Y0A41SK0F_Read_ToF1(h_GP2Y0A41SK0F_t * htof)
{
	if (HAL_ADC_ConfigChannel(htof->hadc, &htof->cConfig_tof1) != HAL_OK) {
		Error_Handler();
	}

	HAL_ADC_Start(htof->hadc);

	if (HAL_ADC_PollForConversion(htof->hadc, HAL_MAX_DELAY) != HAL_OK) {
		Error_Handler();
	}

	htof->adc_val_tof1 = HAL_ADC_GetValue(htof->hadc);
	HAL_ADC_Stop(htof->hadc);
}

/**
 * @brief Read raw ADC value from ToF sensor 2.
 * @param htof Pointer to the GP2Y0A41SK0F handle structure.
 */
void GP2Y0A41SK0F_Read_ToF2(h_GP2Y0A41SK0F_t * htof) {
	if (HAL_ADC_ConfigChannel(htof->hadc, &htof->cConfig_tof2) != HAL_OK) {
		Error_Handler();
	}
	HAL_ADC_Start(htof->hadc);

	if (HAL_ADC_PollForConversion(htof->hadc, HAL_MAX_DELAY) != HAL_OK) {
		Error_Handler();
	}

	htof->adc_val_tof2 = HAL_ADC_GetValue(htof->hadc);
	HAL_ADC_Stop(htof->hadc);
}

/**
 * @brief Compute distances for both ToF sensors based on ADC readings.
 *
 * This function converts ADC values to distances in millimeters using a formula
 * derived from the sensor datasheet.
 *
 * @param htof Pointer to the GP2Y0A41SK0F handle structure.
 *
 * @warning:
 * 	- It expects to use a 12 bits ADC as an imput
 * 	- it ranges from 35 mm to 400 mm
 * @wource:
 * 	- https://github.com/sharpsensoruser/sharp-sensor-demos/blob/master/sharp_gp2y0a41sk0f_demo/sharp_gp2y0a41sk0f_demo.ino
 */
void GP2Y0A41SK0F_get_distance(h_GP2Y0A41SK0F_t * htof)
{
	// Convert to Distance in units of mm
	// by approximating datasheet graph
	// using equation of form: y = a/x + b
	// and two (x,y) points on the graph:
	// (35mm, 3V) and (400mm, 0.3V)
	const float a = 120;
	const float b = 0.03;
	float Vo = 0;

	GP2Y0A41SK0F_Read_ToF1(htof);
	GP2Y0A41SK0F_Read_ToF2(htof);

	Vo = (GP2Y0A41SK0F_ADC_VCC * htof->adc_val_tof1) / GP2Y0A41SK0F_ADC_MAX_VALUE;

	if ( Vo > b ) {
		// Distance measured by ToF1 in mm
		htof->distance_tof1 = (int)( a / (Vo - b) );
	}

	Vo = (5.0 * htof->adc_val_tof2) / GP2Y0A41SK0F_ADC_MAX_VALUE;

	if ( Vo > b ) {
		// Distance measured by ToF2 in mm
		htof->distance_tof2 = (int)( a / (Vo - b) );
	}
}
