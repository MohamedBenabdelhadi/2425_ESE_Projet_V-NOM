/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 * @note
 * Details on the YLIDAR used for this project can be found at:
 * https://www.ydlidar.com/products/view/6.html
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LOGS 0

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
	uint8_t* data_buffer;
	int data_length;
	uint8_t sample_quantity;	// Samples in last received data
	YLIDARX2_sample_t* samples;
} h_YLIDARX2_t;

h_YLIDARX2_t hYLIDAR;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t uartBuffer[USART_BUFFER_SIZE];
uint8_t rxByte; // For single-byte reception
uint16_t bufferIndex = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
 * @brief Transmit a character over UART.
 * @param ch: Character to transmit.
 * @retval int: The transmitted character.
 */
int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);

	return ch;
}

void printData(uint8_t* data, int length)
{
	printf("Data: ");

	for (int i=0; i < length; i++)
	{
		printf("0x%X ", data[i]);

		if (i%10 == 0) printf("\r\n");
	}

	printf("\r\n");
}

uint16_t calculateChecksum(uint8_t *data, uint16_t length)
{
	uint16_t checksum = 0;

	for (int i = 0; i < length; i+=2) // Exclude received checksum bytes
	{
		checksum ^= data[i] | (data[i+1] << 8);
	}

	return checksum;
}

/**
 * @brief Parse and print YDLIDAR X2 scan data.
 * @param data: Pointer to the received data buffer.
 * @retval None
 */
void parseYLIDARData(uint8_t *data)
{
	if (data[0] == YLIDAR_START_BYTE2 && data[1] == YLIDAR_START_BYTE1)
	{
#if (LOGS)
		printf("Started parsing\r\n");
#endif

		// Verify checksum
		uint16_t checksum = data[8] | (data[9] << 8);
		uint16_t calculatedChecksum = calculateChecksum(data, YLIDAR_SAMPLE_BYTE_OFFSET);

		if (calculatedChecksum != checksum)
		{
#if (LOGS)
			printf("Checksum mismatch! Calculated: 0x%X, Received: 0x%X\r\n", calculatedChecksum, checksum);
#endif
			return;
		}

		printData(hYLIDAR.data_buffer, hYLIDAR.data_length);

#if (LOGS)
		// Extract fields in little-endian mode
		uint16_t packetHeader = (data[1] << 8) | data[0];
		uint8_t packageType = data[2] & 0x1;
		uint8_t scan_frequency = (data[2] >> 1)/10;

		printf("Packet Header: 0x%X\r\n", packetHeader);
		printf("Package Type: %s\r\n", YLIDAR_PACKAGE_TYPE(packageType));
		printf("Scan frequency: %d Hz\r\n", scan_frequency);
#endif
		printf("Sample Quantity: %d\r\n", hYLIDAR.sample_quantity);

		uint16_t startAngleRaw = data[4] | (data[5] << 8);
		uint16_t endAngleRaw = data[6] | (data[7] << 8);

		// Calculate starting and ending angles
		float Angle_FSA = (startAngleRaw >> 1) / 64.0f; // Formula: Rshiftbit(FSA) / 64
		float Angle_LSA = (endAngleRaw >> 1) / 64.0f;   // Formula: Rshiftbit(LSA) / 64
#if (LOGS)
		printf("Start Angle: %.2f°, End Angle: %.2f°\r\n", Angle_FSA, Angle_LSA);
#endif

		// Calculate the angle difference
		float diffAngle = (Angle_LSA > Angle_FSA) ? (Angle_LSA - Angle_FSA) : (360.0f + Angle_LSA - Angle_FSA);

		// Process sample data
		printf("Sample Data:\r\n");

		YLIDARX2_sample_t samples[hYLIDAR.sample_quantity];

		for (int i = 0; i < hYLIDAR.sample_quantity; i++)
		{
			samples[i].data = data[10 + i*2] | (data[11 + i*2] << 8);
			samples[i].distance = (uint16_t)((samples[i].data) >> 2);
			samples[i].interference_flag = (samples[i].data) & 0b11; // Lower 2 bits

			// Compute the intermediate angle
			float Angle_i = diffAngle * (float)((i - 1)/(hYLIDAR.sample_quantity-1)) + Angle_FSA;

			// Compute angle correction
			float AngCorrect = 0.0f;

			if (samples[i].distance > 0)
			{
				AngCorrect = atan(21.8f * (155.3f - samples[i].distance)/(155.3f * samples[i].distance) ) * (180.0f / PI);
			}

			samples[i].corrected_angle = Angle_i + AngCorrect;
		}

		hYLIDAR.samples = samples;

		for (int i=0; i < hYLIDAR.sample_quantity; i++)
		{
			printf("Sample %d: Distance = %d mm, ", i + 1, hYLIDAR.samples[i].distance);
#if (LOGS)
			printf("Interference = %d, ", hYLIDAR.samples[i].interference_flag);
#endif
			printf("Corrected Angle = %.2f°\r\n", hYLIDAR.samples[i].corrected_angle);
		}
	}
	else
	{
		printf("Invalid start bytes!\r\n");
	}
}


/**
 * @brief UART receive complete callback.
 * @param huart: Pointer to the UART handle
 * @retval None
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART3)
	{
		// Add received byte to the buffer
		uartBuffer[bufferIndex++] = rxByte;

		// Check for start bytes and process data only if a full packet is received
		if (bufferIndex >= 2)
		{
			if(uartBuffer[0] == YLIDAR_START_BYTE2 && uartBuffer[1] == YLIDAR_START_BYTE1)
			{
				if (bufferIndex >= YLIDAR_PACKET_HEADER_LENGTH) // Minimum packet size
				{
					// Extract sample quantity
					hYLIDAR.sample_quantity = uartBuffer[3];
					uint16_t expectedLength = YLIDAR_PACKET_HEADER_LENGTH + (hYLIDAR.sample_quantity * 2);

					// Process only when the full packet is received
					if (bufferIndex >= expectedLength)
					{
						hYLIDAR.data_buffer = uartBuffer;
						hYLIDAR.data_length = bufferIndex;

						parseYLIDARData(hYLIDAR.data_buffer);

						// Reset the buffer
						bufferIndex = 0;
						memset(uartBuffer, 0, sizeof(uartBuffer));
					}
				}
			}
			else
			{
				// Shift buffer to discard invalid start bytes
				memmove(uartBuffer, uartBuffer + 1, --bufferIndex);
			}
		}

		if (bufferIndex >= USART_BUFFER_SIZE)
		{
			// Reset buffer if overflow occurs
			bufferIndex = 0;
#if (LOGS)
			printf("Buffer overflow! Clearing buffer.\r\n");
#endif
		}

		// Restart reception for the next byte
		HAL_UART_Receive_IT(&huart3, &rxByte, 1);
	}
}

/**
 * @brief UART error callback.
 * @param huart: Pointer to the UART handle
 * @retval None
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART3)
	{
		if (HAL_UART_GetError(huart) & HAL_UART_ERROR_PE)
			printf("Parity Error!\r\n");
		if (HAL_UART_GetError(huart) & HAL_UART_ERROR_NE)
			printf("Noise Error!\r\n");
		if (HAL_UART_GetError(huart) & HAL_UART_ERROR_FE)
			printf("Framing Error!\r\n");
		if (HAL_UART_GetError(huart) & HAL_UART_ERROR_ORE)
			printf("Overrun Error!\r\n");

		// Restart UART reception after error
		HAL_UART_Receive_IT(&huart3, &rxByte, 1);
	}
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART2_UART_Init();
	MX_USART3_UART_Init();
	/* USER CODE BEGIN 2 */
	printf("\r\n***** TEST YLIDAR X2 *****\r\n");

	/* Clear the UART buffer to avoid receiving residual data */
	//HAL_UART_Abort(&huart1);

	// Start UART reception in interrupt mode (1 byte at a time)
	HAL_UART_Receive_IT(&huart3, &rxByte, 1);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
	RCC_OscInitStruct.PLL.PLLN = 85;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
