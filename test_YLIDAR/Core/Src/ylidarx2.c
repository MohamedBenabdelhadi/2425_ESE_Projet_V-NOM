/*
 * ylidarx2.c
 *
 *  Created on: Jan 3, 2025
 *      Author: oliver
 */

#include "ylidarx2.h"
#include <string.h>

#define DEBUG 1 // Enable debug logs

#if DEBUG
#include <stdio.h>
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

#define FRAME_HEADER 0xAA  // Example header byte
#define FRAME_LENGTH 9     // Adjust this based on your LIDAR's datasheet

// Initialize the LIDAR handle
void YDLidar_Init(YDLidar_t *lidar, UART_HandleTypeDef *huart) {
    lidar->uart = huart;
    lidar->rxIndex = 0;
    lidar->rawDataLength = 0;
    memset(lidar->rxBuffer, 0, sizeof(lidar->rxBuffer));
    memset(lidar->rawData, 0, sizeof(lidar->rawData));

    // Start receiving data
    HAL_UART_Receive_IT(lidar->uart, lidar->rxBuffer, 1);
    DEBUG_PRINT("Initialization complete\r\n");
}

// Handle received byte (called from UART interrupt callback)
void YDLidar_UARTCallback(YDLidar_t *lidar) {
    lidar->rawData[lidar->rawDataLength++] = lidar->rxBuffer[0];

    // Check for a complete frame
    if (lidar->rawDataLength >= FRAME_LENGTH) {
        if (lidar->rawData[0] == FRAME_HEADER && YDLidar_ValidateChecksum(lidar->rawData, FRAME_LENGTH)) {
            // Process the valid frame
            YDLidar_ProcessData(lidar, lidar->rawData);
        }
        // Shift the buffer for the next potential frame
        memmove(lidar->rawData, lidar->rawData + 1, --lidar->rawDataLength);
    }

    HAL_UART_Receive_IT(lidar->uart, lidar->rxBuffer, 1);
}

// Validate checksum of a LIDAR frame
bool YDLidar_ValidateChecksum(const uint8_t *data, uint16_t length) {
    uint8_t checksum = 0;
    for (uint16_t i = 0; i < length - 1; i++) {
        checksum ^= data[i];
    }
    return checksum == data[length - 1];
}

// Process the received LIDAR data
bool YDLidar_ProcessData(YDLidar_t *lidar, const uint8_t *frame) {
    // Extract information from the frame
    uint16_t distance = (frame[2] << 8) | frame[3];  // Example: Distance is bytes 2-3
    uint16_t angle_raw = (frame[4] << 8) | frame[5]; // Example: Angle is bytes 4-5
    uint8_t intensity = frame[6];                   // Example: Intensity is byte 6

    // Convert angle to degrees
    float angle = (angle_raw / 64.0f) - 180.0f; // Adjust as per datasheet format

    // Convert distance to millimeters
    float distance_mm = distance / 4.0f; // Adjust as per datasheet format

    // Ensure the values are within valid ranges
    if (angle >= -180.0f && angle <= 180.0f && distance_mm >= 50.0f && distance_mm <= 12000.0f) {
        // Print or store the data point
        printf("Angle: %.2f°, Distance: %.2f mm, Intensity: %u\r\n", angle, distance_mm, intensity);
        return true;
    } else {
        printf("Invalid data: Angle %.2f°, Distance %.2f mm\r\n", angle, distance_mm);
        return false;
    }
}
