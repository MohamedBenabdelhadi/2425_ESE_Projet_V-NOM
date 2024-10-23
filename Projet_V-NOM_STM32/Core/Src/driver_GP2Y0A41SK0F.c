/*
 * driver_GP2Y0A41SK0F.c
 *
 *  Created on: Oct 22, 2024
 *      Author: oliver
 */

#include "driver_GP2Y0A41SK0F.h"


/**
 * Compute the measured distance by the sensor.
 * Warning:
 * 	- It's expected to use a 12 bits ADC as an imput
 * Source:
 * 	- https://github.com/sharpsensoruser/sharp-sensor-demos/blob/master/sharp_gp2y0a41sk0f_demo/sharp_gp2y0a41sk0f_demo.ino
 */
void GP2Y0A41SK0F_get_distance(h_GP2Y0A41SK0F_t * htof)
{
	// Convert to Distance in units of mm
	// by approximating datasheet graph
	// using equation of form: y = a/x + b
	// and two (x,y) points on the graph:
	// (60mm, 2.02V) and (300mm, 0.435V)
	const float a = 118.875;
	const float b = 0.03875;
	float Vo = (5.0 * htof->adc_val) / GP2Y0A41SK0F_ADC_MAX_VALUE;

	if ( Vo > b ) {
		htof->distance = a / (Vo - b);
	}
}
