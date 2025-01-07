/**
 * @file ylidarx2.c
 * @brief Implementation of the YDLIDAR X2 driver.
 *
 * Handles initialization, data reception via DMA, and
 * processing of LIDAR frames.
 *
 * @date Jan 3, 2025
 * @author Oliver
 */

#include "ylidarx2.h"
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
static uint16_t
