/**
 * @file driver_YLIDARX2.h
 * @brief Header file for the YDLIDAR X2 driver.
 *
 * Provides definitions and function prototypes for initializing,
 * processing, and handling YDLIDAR X2 data using DMA.
 *
 * @date Jan 3, 2025
 * @author Oliver
 */

#ifndef INC_DRIVER_YLIDARX2_H_
#define INC_DRIVER_YLIDARX2_H_

#include "usart.h"
#include <stdint.h>
#include <stdbool.h>

#define YLIDARX2_DMA_BUFFER_SIZE 2000 /**< DMA buffer size for UART reception */
#define YLIDARX2_MAX_POINTS      1000 /**< Maximum number of processed points stored */
#define PACKET_HEADER            0x55AA /**< Frame header identifier */
#define FRAME_LENGTH_MIN         7 /**< Minimum frame length in bytes */

/**
 * @brief Structure to hold individual LIDAR point data.
 */
typedef struct {
    float angle; /**< Angle in degrees */
    float distance; /**< Distance in millimeters */
    uint8_t intensity; /**< Intensity of the signal */
} YLIDARX2_Point_t;

/**
 * @brief Structure to hold LIDAR state and data.
 */
typedef struct {
    UART_HandleTypeDef *uart; /**< UART handle for communication */
    uint8_t dmaBuffer[YLIDARX2_DMA_BUFFER_SIZE]; /**< DMA buffer for raw data */
    uint16_t currentIndex; /**< Current index in the DMA buffer */
    YLIDARX2_Point_t points[YLIDARX2_MAX_POINTS]; /**< Array of processed points */
    uint16_t pointIndex; /**< Index for storing the next processed point */
} YLIDARX2_t;

/**
 * @brief Initialize the YDLIDAR X2 driver with DMA.
 *
 * @param lidar Pointer to the YDLIDARX2_t structure.
 * @param huart UART handle for communication.
 */
void YLIDARX2_InitDMA(YLIDARX2_t *lidar, UART_HandleTypeDef *huart);

/**
 * @brief Process the first half of the DMA buffer.
 *
 * @param lidar Pointer to the YDLIDARX2_t structure.
 */
void YLIDARX2_ProcessDMAHalfComplete(YLIDARX2_t *lidar);

/**
 * @brief Process the second half of the DMA buffer.
 *
 * @param lidar Pointer to the YDLIDARX2_t structure.
 */
void YLIDARX2_ProcessDMAComplete(YLIDARX2_t *lidar);

/**
 * @brief Process a portion of the DMA buffer.
 *
 * @param lidar Pointer to the YDLIDARX2_t structure.
 * @param start Start index of the buffer to process.
 * @param end End index of the buffer to process.
 */
void YLIDARX2_ProcessBuffer(YLIDARX2_t *lidar, uint16_t start, uint16_t end);

/**
 * @brief Validate the checksum of a LIDAR frame.
 *
 * @param data Pointer to the frame data.
 * @param length Length of the frame.
 * @return True if the checksum is valid, false otherwise.
 */
bool YLIDARX2_ValidateChecksum(const uint8_t *data, uint16_t length);

/**
 * @brief Process a single LIDAR frame.
 *
 * Extracts angle, distance, and intensity from the frame.
 *
 * @param lidar Pointer to the YDLIDARX2_t structure.
 * @param frame Pointer to the frame data.
 */
void YLIDARX2_ProcessFrame(YLIDARX2_t *lidar, const uint8_t *frame);

#endif /* INC_DRIVER_YLIDARX2_H_ */
