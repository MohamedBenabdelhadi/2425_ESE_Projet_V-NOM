/*
 * driver_YLIDARX2.h
 *
 *  Created on: Dec 11, 2024
 *      Author: oliver
 */

#ifndef INC_DRIVER_YLIDARX2_H_
#define INC_DRIVER_YLIDARX2_H_

#include <inttypes.h>

#define USART_BUFFER_SIZE 1024 // Increase buffer size to handle more data

// YLIDAR2 constants
#define YLIDAR_START_BYTE1 0x55
#define YLIDAR_START_BYTE2 0xAA
#define YLIDAR_PACKET_HEADER_LENGTH 26
#define YLIDAR_SAMPLE_BYTE_OFFSET 8
#define YLIDAR_PACKAGE_TYPE(byte)  \
		((byte) & 0x01 ? "beginning of a lap of data" : "point cloud data packet")

// Math constants
#define PI 3.14159265359

// Printf binary tools
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
		((byte) & 0x80 ? '1' : '0'), \
		((byte) & 0x40 ? '1' : '0'), \
		((byte) & 0x20 ? '1' : '0'), \
		((byte) & 0x10 ? '1' : '0'), \
		((byte) & 0x08 ? '1' : '0'), \
		((byte) & 0x04 ? '1' : '0'), \
		((byte) & 0x02 ? '1' : '0'), \
		((byte) & 0x01 ? '1' : '0')

#define BINARY_TO_TF(bit)  \
		((bit) & 1 ? "TRUE" : 'FALSE')

typedef struct {
	uint16_t data;
	uint16_t distance;
	uint8_t interference_flag;
	float corrected_angle;
} YLIDARX2_sample_t;

typedef struct {
	uint8_t* uart_buffer;
	uint8_t* data_buffer;
	int data_length;
	uint8_t sample_quantity;	// Samples in last received data
	YLIDARX2_sample_t* samples;
} h_YLIDARX2_t;


/*
 * driver_YLIDARX2.c
 *
 *  Created on: Dec 11, 2024
 *      Author: oliver
 */

#include "driver_YLIDARX2.h"
#include "usart.h"


void YLIDARX2_PrintData(h_YLIDARX2_t * hYLIDAR);
void YLIDARX2_PrintSamples(h_YLIDARX2_t * hYLIDAR);
uint16_t YLIDARX2_CalculateChecksum(uint8_t *data, uint16_t length);
void YLIDARX2_ParseData(h_YLIDARX2_t * hYLIDAR);
void YLIDARX2_UART_irq(h_YLIDARX2_t * hYLIDAR);

#endif /* INC_DRIVER_YLIDARX2_H_ */
