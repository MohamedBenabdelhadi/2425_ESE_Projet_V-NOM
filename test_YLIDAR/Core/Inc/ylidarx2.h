/*
 * ylidarx2.h
 *
 *  Created on: Jan 3, 2025
 *      Author: oliver
 */

#ifndef INC_YLIDARX2_H_
#define INC_YLIDARX2_H_

#include "usart.h"
#include <stdint.h>
#include <stdbool.h>

// LIDAR properties
typedef struct {
    UART_HandleTypeDef *uart;         // UART handle
    uint8_t rxBuffer[64];            // Temporary reception buffer
    uint16_t rxIndex;                // Reception buffer index
    uint8_t rawData[2048];           // Data buffer for raw LIDAR frames
    uint16_t rawDataLength;          // Length of raw data
} YDLidar_t;

// Initialization and setup
void YDLidar_Init(YDLidar_t *lidar, UART_HandleTypeDef *huart);

// Data processing
bool YDLidar_ProcessData(YDLidar_t *lidar, const uint8_t *frame);

// UART interrupt handling
void YDLidar_UARTCallback(YDLidar_t *lidar);

// Utility functions
bool YDLidar_ValidateChecksum(const uint8_t *data, uint16_t length);

#endif /* INC_YLIDARX2_H_ */
