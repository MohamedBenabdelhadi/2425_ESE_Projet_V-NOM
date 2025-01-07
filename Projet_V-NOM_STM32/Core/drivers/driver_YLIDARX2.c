/**
 * @file driver_YLIDARX2.c
 * @brief Implementation of the YDLIDAR X2 driver.
 *
 * Handles initialization, data reception via DMA, and
 * processing of LIDAR frames.
 *
 * @date Jan 3, 2025
 * @author Oliver
 */

#include "driver_YLIDARX2.h"
#include <string.h>
#include <stdio.h>

#define DEBUG 1 /**< Enable or disable debug logs */

#if DEBUG
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

/**
 * @brief Extract a 16-bit unsigned integer from the byte stream.
 *
 * @param data Pointer to the byte stream.
 * @return Extracted 16-bit unsigned integer.
 */
static uint16_t extract_uint16(const uint8_t *data) {
    return (data[1] << 8) | data[0];
}

void YLIDARX2_InitDMA(h_YLIDARX2_t *lidar, UART_HandleTypeDef *huart) {
    lidar->uart = huart;
    lidar->currentIndex = 0;
    lidar->pointIndex = 0;
    memset(lidar->dmaBuffer, 0, sizeof(lidar->dmaBuffer));

    HAL_UART_Receive_DMA(lidar->uart, lidar->dmaBuffer, YLIDARX2_DMA_BUFFER_SIZE);

    DEBUG_PRINT("YLIDARX2 DMA Initialized\r\n");
}

void YLIDARX2_ProcessDMAHalfComplete(h_YLIDARX2_t *lidar) {
    YLIDARX2_ProcessBuffer(lidar, 0, YLIDARX2_DMA_BUFFER_SIZE / 2);
}

void YLIDARX2_ProcessDMAComplete(h_YLIDARX2_t *lidar) {
    YLIDARX2_ProcessBuffer(lidar, YLIDARX2_DMA_BUFFER_SIZE / 2, YLIDARX2_DMA_BUFFER_SIZE);
}

void YLIDARX2_ProcessBuffer(h_YLIDARX2_t *lidar, uint16_t start, uint16_t end) {
    for (uint16_t i = start; i < end - FRAME_LENGTH_MIN; ) {
        uint16_t header = extract_uint16(&lidar->dmaBuffer[i]);

        if (header == PACKET_HEADER) {
            uint8_t ct = lidar->dmaBuffer[i + 2];
            uint8_t lsn = lidar->dmaBuffer[i + 3];
            uint16_t fsa = extract_uint16(&lidar->dmaBuffer[i + 4]);
            uint16_t lsa = extract_uint16(&lidar->dmaBuffer[i + 6]);
            uint16_t checksum = extract_uint16(&lidar->dmaBuffer[i + 8 + (lsn * 2)]);

            uint16_t calculatedChecksum = 0;
            for (uint16_t j = i; j < i + 8 + (lsn * 2); j++) {
                calculatedChecksum ^= lidar->dmaBuffer[j];
            }

            if (calculatedChecksum == checksum) {
                float startAngle = (fsa >> 1) / 64.0f;
                float endAngle = (lsa >> 1) / 64.0f;
                float angleIncrement = (endAngle - startAngle) / (lsn - 1);

                for (uint8_t j = 0; j < lsn; j++) {
                    if (lidar->pointIndex >= YLIDARX2_MAX_POINTS) {
                        lidar->pointIndex = 0;
                    }

                    uint16_t distance = extract_uint16(&lidar->dmaBuffer[i + 8 + (j * 2)]);
                    float angle = startAngle + (j * angleIncrement) - 180;
                    lidar->points[lidar->pointIndex].angle = angle;
                    lidar->points[lidar->pointIndex].distance = distance / 4.0f;
                    lidar->points[lidar->pointIndex].intensity = (distance & 0x01) ? 1 : 0;

                    DEBUG_PRINT("Point %d: Angle %.2f°, Distance %.2f mm, Intensity %u\r\n",
                                lidar->pointIndex,
                                lidar->points[lidar->pointIndex].angle,
                                lidar->points[lidar->pointIndex].distance,
                                lidar->points[lidar->pointIndex].intensity);

                    lidar->pointIndex++;
                }

                i += 10 + (lsn * 2); // Move to next packet
            } else {
                DEBUG_PRINT("Checksum failed at index %d\r\n", i);
                i++;
            }
        } else {
            i++;
        }
    }
}
