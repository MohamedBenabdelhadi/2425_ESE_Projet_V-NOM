/**
 * @file driver_ADXL343.c
 * @brief C code file for ADXL343 accelerometer driver.
 * @author Oliver
 * @date December 22, 2024
 *************************************************************
 * @warning
 * - The maximum SPI clock speed for the ADXL343 is 5 MHz
 *   with a maximum bus load capacitance of 100 pF.
 */
#include "driver_ADXL343.h"
#include "spi.h"
#include "gpio.h"

#define DEBUG 1 /**< Enable debug logs */

#if DEBUG
#include <stdio.h>
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

/**
 * @brief Sends a command and data to the ADXL343 over SPI.
 * @param address The address of the register to write to.
 * @param data Pointer to the data to send.
 * @param length Number of bytes to send.
 */
void spiWrite(uint8_t address, uint8_t *data, uint16_t length)
{
    HAL_StatusTypeDef status;

    // Set the write command (clear MSB of the register address)
    uint8_t writeAddress = address & 0x7F;

    // Pull CS low to start the SPI transaction
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);

    // Transmit the address
    status = HAL_SPI_Transmit(&hspi1, &writeAddress, 1, HAL_MAX_DELAY);
    if (status != HAL_OK)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET); // Set CS high on error
        DEBUG_PRINT("SPI Write Error during address transmission!\n");
        Error_Handler();
    }

    // Transmit the data
    status = HAL_SPI_Transmit(&hspi1, data, length, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET); // Set CS high after transaction

    if (status != HAL_OK)
    {
        DEBUG_PRINT("SPI Write Error during data transmission!\n");
        Error_Handler();
    }
}

/**
 * @brief Reads data from the ADXL343 over SPI.
 * @param address The address of the register to read from.
 * @param data Pointer to the buffer to store the received data.
 * @param length Number of bytes to read.
 */
void spiRead(uint8_t address, uint8_t *data, uint16_t length)
{
    HAL_StatusTypeDef status;

    // Pull CS low to start the SPI transaction
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);

    // Transmit the address
    status = HAL_SPI_Transmit(&hspi1, &address, 1, HAL_MAX_DELAY);
    if (status != HAL_OK)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET); // Set CS high on error
        DEBUG_PRINT("SPI Read Error during address transmission!\n");
        Error_Handler();
    }

    // Receive the data
    status = HAL_SPI_Receive(&hspi1, data, length, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET); // Set CS high after transaction

    if (status != HAL_OK)
    {
        DEBUG_PRINT("SPI Read Error during data reception!\n");
        Error_Handler();
    }
}

/**
 * @brief Writes data to a specific register of the ADXL343.
 * @param reg The register address to write to.
 * @param values Pointer to the data to write.
 * @param length Number of bytes to write.
 */
void ADXL343_writeRegister(uint8_t reg, uint8_t *values, uint16_t length)
{
    uint8_t writeAddress = reg & ~(0x80); // Write mode
    spiWrite(writeAddress, values, length);
}

/**
 * @brief Reads data from a specific register of the ADXL343.
 * @param reg The register address to read from.
 * @param data Pointer to the buffer to store the read data.
 * @param num Number of bytes to read.
 */
void ADXL343_readRegister(uint8_t reg, uint8_t *data, uint16_t num)
{
    if (num > 1)
        reg |= 0x40; // Enable multiple-byte read
    reg |= 0x80;    // Set read mode

    spiRead(reg, data, num);
}

/**
 * @brief Initializes the ADXL343 accelerometer.
 * @param hadxl Pointer to the ADXL343 handle structure.
 * @note
 * - Verifies device ID and configures for single-tap detection.
 */
void ADXL343_Init(h_ADXL343_t *hadxl)
{
    DEBUG_PRINT("Setting CSn\n");
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET); // Set CS high

    uint8_t deviceID;
    ADXL343_readRegister(ADXL343_DEVID, &deviceID, 1);

    if (deviceID == 0xE5)
    {
        DEBUG_PRINT("ADXL343 detected successfully! Device ID: 0x%02X\n", deviceID);
    }
    else
    {
        DEBUG_PRINT("Failed to detect ADXL343. Read Device ID: 0x%02X\n", deviceID);
    }

    uint8_t val = 0x00;
    ADXL343_writeRegister(ADXL343_POWER_CTL, &val, 1); // Standby mode

    val = 0x08;
    ADXL343_writeRegister(ADXL343_POWER_CTL, &val, 1); // Measure mode

    val = 0x01;
    ADXL343_writeRegister(ADXL343_DATA_FORMAT, &val, 1); // ±4g range

    ADXL343_ConfigureTap(hadxl);
}

/**
 * @brief Configures single-tap detection for the ADXL343.
 * @param hadxl Pointer to the ADXL343 handle structure.
 * @note
 * - Configures tap threshold, duration, and interrupt mapping.
 */
void ADXL343_ConfigureTap(h_ADXL343_t *hadxl)
{
    uint8_t val;

    // Set tap threshold
    val = 50; // 50 * 0.0625g per LSB
    ADXL343_writeRegister(ADXL343_THRESH_TAP, &val, 1);

    // Set tap duration
    val = 30; // 30 ms
    ADXL343_writeRegister(ADXL343_DUR, &val, 1);

    // Enable tap detection on all axes
    val = 0x07; // X, Y, Z enabled
    ADXL343_writeRegister(ADXL343_TAP_AXES, &val, 1);

    // Enable single-tap interrupt
    val = 0x40; // SINGLE_TAP bit
    ADXL343_writeRegister(ADXL343_INT_ENABLE, &val, 1);

    // Map single-tap interrupt to INT1
    val = 0x00; // Map to INT1
    ADXL343_writeRegister(ADXL343_INT_MAP, &val, 1);

    // Configure GPIO for INT1
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = AG_INT1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // Enable EXTI interrupt
    HAL_NVIC_SetPriority(EXTI3_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);

    DEBUG_PRINT("ADXL343 configured for single-tap detection.\n");
}

/**
 * @brief Reads the acceleration data from the ADXL343.
 * @param hadxl Pointer to the ADXL343 handle structure.
 * @note
 * - Converts raw data to acceleration in g.
 */
void ADXL343_get_Acceleration(h_ADXL343_t *hadxl)
{
    ADXL343_readRegister(ADXL343_DATAX0, hadxl->RxData, 6);

    int16_t RAWX = (hadxl->RxData[1] << 8) | hadxl->RxData[0];
    int16_t RAWY = (hadxl->RxData[3] << 8) | hadxl->RxData[2];
    int16_t RAWZ = (hadxl->RxData[5] << 8) | hadxl->RxData[4];

    hadxl->xg = (float)RAWX / 128;
    hadxl->yg = (float)RAWY / 128;
    hadxl->zg = (float)RAWZ / 128;

    DEBUG_PRINT(
        "Xg: %.4f\n"
        "Yg: %.4f\n"
        "Zg: %.4f\n",
        hadxl->xg, hadxl->yg, hadxl->zg);
}
