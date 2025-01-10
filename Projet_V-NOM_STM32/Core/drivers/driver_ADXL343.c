/**
 * @file driver_ADXL343.c
 * @brief C code file for ADXL343 accelerometer driver.
 * @author Oliver
 * @date December 22, 2024
 *************************************************************
 * @warning
 * - The maximum SPI clock speed for the ADXL343 is 5 MHz
 * 	 with a maximum bus load capacitance of 100 pF.
 */
#include "driver_ADXL343.h"
#include "spi.h"

#define DEBUG 1 // Enable debug logs

#if DEBUG
#include <stdio.h>
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

//////////////////////////////////////////////////
//////////////////// ADXL343 ////////////////////
////////////////////////////////////////////////

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
		printf("SPI Write Error during address transmission!\n");
		Error_Handler();
	}

	// Transmit the data
	status = HAL_SPI_Transmit(&hspi1, data, length, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET); // Set CS high after transaction

	if (status != HAL_OK)
	{
		printf("SPI Write Error during data transmission!\n");
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
		printf("SPI Read Error during address transmission!\n");
		Error_Handler();
	}

	// Receive the data
	status = HAL_SPI_Receive(&hspi1, data, length, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET); // Set CS high after transaction

	if (status != HAL_OK)
	{
		printf("SPI Read Error during data reception!\n");
		Error_Handler();
	}
}

/**
 * @brief Writes data to a specific register of the ADXL343.
 * @param reg The register address to write to.
 * @param values Pointer to the data to write.
 * @param length Number of bytes to write.
 */
void ADXL343_writeRegister(uint8_t reg, uint8_t * values, uint16_t length) {
	// Setting R/W = 0, i.e.: Write Mode
	uint8_t writeAddress = reg & ~(0x80);

	spiWrite(writeAddress, values, length);
}

/**
 * @brief Reads data from a specific register of the ADXL343.
 * @param reg The register address to read from.
 * @param data Pointer to the buffer to store the read data.
 * @param num Number of bytes to read.
 */
void ADXL343_readRegister(uint8_t reg, uint8_t *data, uint16_t num) {
	if (reg > 63)
		reg = 63;

	// Multiple Byte Read Settings
	if (num > 1)
		reg |= 0x40;
	else
		reg &= ~(0x40);

	// Setting R/W = 1, i.e.: Read Mode
	reg |= (0x80);

	spiRead(reg, data, num);
}

/**
 * @brief Initializes the ADXL343 accelerometer.
 * @param hadxl Pointer to the ADXL343 handle structure.
 * @note
 * - Useful resource: https://controllerstech.com/adxl345-accelerometer-using-stm32/
 */
void ADXL343_Init(h_ADXL343_t * hadxl) {
	DEBUG_PRINT("Setting CSn\r\n");
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET); // Set CS high

	uint8_t deviceID;
	ADXL343_readRegister(ADXL343_DEVID, &deviceID, 1);

	if (deviceID == 0xE5) { // Device ID for ADXL343
		printf("ADXL343 detected successfully! Device ID: 0x%02X\r\n", deviceID);
	} else {
		printf("Failed to detect ADXL343. Read Device ID: 0x%02X\r\n", deviceID);
	}

	if (deviceID == 0xE5)
	{
		uint8_t val = 0x00;
		ADXL343_writeRegister (ADXL343_POWER_CTL, &val, 1);  // reset all bits; standby

		val = 0x08;
		ADXL343_writeRegister (ADXL343_POWER_CTL, &val, 1);  // measure=1 and wake up 8hz

		val = 0x01;
		ADXL343_writeRegister (ADXL343_DATA_FORMAT, &val, 1);  // 10bit data, range= +- 4g
	}

	ADXL343_ConfigureTap(hadxl);
}

void ADXL343_ConfigureTap(h_ADXL343_t *hadxl) {
   uint8_t val;

   // Seuil de tap (ajustez selon vos besoins)
   val = 50; // Seuil de 50 (0.0625g par incrément)
   ADXL343_writeRegister(ADXL343_THRESH_TAP, &val, 1);

   // Durée du tap
   val = 30; // 30 ms
   ADXL343_writeRegister(ADXL343_DUR, &val, 1);

   // Active les axes pour la détection de tap (X, Y, Z)
   val = 0x07; // Active X, Y, Z
   ADXL343_writeRegister(ADXL343_TAP_AXES, &val, 1);

   // Active l'interruption de tap sur INT1
   val = 0x40; // Single tap interrupt
   ADXL343_writeRegister(ADXL343_INT_ENABLE, &val, 1);
}

/**
 * @brief Reads the acceleration data from the ADXL343.
 * @param hadxl Pointer to the ADXL343 handle structure.
 * @note
 * - Useful resource: https://controllerstech.com/adxl345-accelerometer-using-stm32/
 */
void ADXL343_get_Acceleration(h_ADXL343_t * hadxl)
{
	ADXL343_readRegister(ADXL343_DATAX0, hadxl->RxData, 6);

	int16_t RAWX = ((hadxl->RxData[1]<<8)|hadxl->RxData[0]);
	int16_t RAWY = ((hadxl->RxData[3]<<8)|hadxl->RxData[2]);
	int16_t RAWZ = ((hadxl->RxData[5]<<8)|hadxl->RxData[4]);

	hadxl->xg = (float)RAWX/128;
	hadxl->yg = (float)RAWY/128;
	hadxl->zg = (float)RAWZ/128;

	DEBUG_PRINT(
			"Xg: %.4f\r\n"
			"Yg: %.4f\r\n"
			"Zg: %.4f\r\n",
			hadxl->xg, hadxl->yg, hadxl->zg);
}
