/*
 * driver_GP2Y0A41SK0F.h
 *
 *  Created on: Oct 22, 2024
 *      Author: oliver
 */

#include <stdlib.h>
#include <inttypes.h>
#include "adc.h"

#define GP2Y0A41SK0F_ADC_BITS 12
#define GP2Y0A41SK0F_ADC_MAX_VALUE ((1 << GP2Y0A41SK0F_ADC_BITS) -1)

typedef struct
{
	ADC_HandleTypeDef * hadc;
	ADC_ChannelConfTypeDef * cConfig_tof1;
	ADC_ChannelConfTypeDef * cConfig_tof2;
	uint32_t adc_val_tof1;
	uint32_t adc_val_tof2;
	int distance_tof1;
	int distance_tof2;
} h_GP2Y0A41SK0F_t;


void GP2Y0A41SK0F_Init(h_GP2Y0A41SK0F_t * htof);
void GP2Y0A41SK0F_Read_ToF1(h_GP2Y0A41SK0F_t * htof);
void GP2Y0A41SK0F_Read_ToF2(h_GP2Y0A41SK0F_t * htof);
void GP2Y0A41SK0F_get_distance(h_GP2Y0A41SK0F_t * htof);
