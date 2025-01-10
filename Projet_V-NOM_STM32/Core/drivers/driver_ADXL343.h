/**
 * @file driver_ADXL343.h
 * @brief Header file for ADXL343 accelerometer driver.
 * @author Oliver
 * @date December 22, 2024
 */

#ifndef DRIVER_ADXL343_H
#define DRIVER_ADXL343_H

#include "stm32g4xx_hal.h"
#include <stdint.h>

/**
 * @brief ADXL343 register addresses.
 */
typedef enum {
    ADXL343_DEVID = 0x00,         /**< Device ID */
    ADXL343_THRESH_TAP = 0x1D,   /**< Tap threshold */
    ADXL343_OFSX = 0x1E,         /**< X-axis offset */
    ADXL343_OFSY = 0x1F,         /**< Y-axis offset */
    ADXL343_OFSZ = 0x20,         /**< Z-axis offset */
    ADXL343_DUR = 0x21,          /**< Tap duration */
    ADXL343_LATENT = 0x22,       /**< Tap latency */
    ADXL343_WINDOW = 0x23,       /**< Tap window */
    ADXL343_THRESH_ACT = 0x24,   /**< Activity threshold */
    ADXL343_THRESH_INACT = 0x25, /**< Inactivity threshold */
    ADXL343_TIME_INACT = 0x26,   /**< Inactivity time */
    ADXL343_ACT_INACT_CTL = 0x27,/**< Axis enable control for activity and inactivity detection */
    ADXL343_THRESH_FF = 0x28,    /**< Free-fall threshold */
    ADXL343_TIME_FF = 0x29,      /**< Free-fall time */
    ADXL343_TAP_AXES = 0x2A,     /**< Axis control for single/double tap */
    ADXL343_ACT_TAP_STATUS = 0x2B,/**< Source of single/double tap */
    ADXL343_BW_RATE = 0x2C,      /**< Data rate and power mode control */
    ADXL343_POWER_CTL = 0x2D,    /**< Power-saving features control */
    ADXL343_INT_ENABLE = 0x2E,   /**< Interrupt enable control */
    ADXL343_INT_MAP = 0x2F,      /**< Interrupt mapping control */
    ADXL343_INT_SOURCE = 0x30,   /**< Source of interrupts */
    ADXL343_DATA_FORMAT = 0x31,  /**< Data format control */
    ADXL343_DATAX0 = 0x32,       /**< X-Axis Data 0 */
    ADXL343_DATAX1 = 0x33,       /**< X-Axis Data 1 */
    ADXL343_DATAY0 = 0x34,       /**< Y-Axis Data 0 */
    ADXL343_DATAY1 = 0x35,       /**< Y-Axis Data 1 */
    ADXL343_DATAZ0 = 0x36,       /**< Z-Axis Data 0 */
    ADXL343_DATAZ1 = 0x37,       /**< Z-Axis Data 1 */
    ADXL343_FIFO_CTL = 0x38,     /**< FIFO control */
    ADXL343_FIFO_STATUS = 0x39   /**< FIFO status */
} ADXL343_Register_t;

/**
 * @brief Handle structure for the ADXL343.
 */
typedef struct {
    uint8_t RxData[6]; /**< Raw data buffer for accelerometer readings. */
    float xg;          /**< Acceleration in X-axis (g). */
    float yg;          /**< Acceleration in Y-axis (g). */
    float zg;          /**< Acceleration in Z-axis (g). */
} h_ADXL343_t;

/**
 * @brief Initializes the ADXL343 accelerometer.
 * @param hadxl Pointer to the ADXL343 handle structure.
 */
void ADXL343_Init(h_ADXL343_t *hadxl);

/**
 * @brief Configures single-tap detection for the ADXL343.
 * @param hadxl Pointer to the ADXL343 handle structure.
 */
void ADXL343_ConfigureTap(h_ADXL343_t *hadxl);

/**
 * @brief Reads the acceleration data from the ADXL343.
 * @param hadxl Pointer to the ADXL343 handle structure.
 */
void ADXL343_get_Acceleration(h_ADXL343_t *hadxl);

/**
 * @brief Writes data to a specific register of the ADXL343.
 * @param reg The register address to write to.
 * @param values Pointer to the data to write.
 * @param length Number of bytes to write.
 */
void ADXL343_writeRegister(uint8_t reg, uint8_t *values, uint16_t length);

/**
 * @brief Reads data from a specific register of the ADXL343.
 * @param reg The register address to read from.
 * @param data Pointer to the buffer to store the read data.
 * @param num Number of bytes to read.
 */
void ADXL343_readRegister(uint8_t reg, uint8_t *data, uint16_t num);

/**
 * @brief Sends a command and data to the ADXL343 over SPI.
 * @param address The address of the register to write to.
 * @param data Pointer to the data to send.
 * @param length Number of bytes to send.
 */
void spiWrite(uint8_t address, uint8_t *data, uint16_t length);

/**
 * @brief Reads data from the ADXL343 over SPI.
 * @param address The address of the register to read from.
 * @param data Pointer to the buffer to store the received data.
 * @param length Number of bytes to read.
 */
void spiRead(uint8_t address, uint8_t *data, uint16_t length);

#endif /* DRIVER_ADXL343_H */
