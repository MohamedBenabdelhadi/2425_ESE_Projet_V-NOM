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
#define USART_BUFFER_SIZE 0x800

// YLIDAR2 constants
#define YLIDAR_START_BYTE1 0x55
#define YLIDAR_START_BYTE2 0xAA
#define YLIDAR_SAMPLE_BYTE_OFFSET 8

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

/**
 * @brief Parse and print YDLIDAR X2 scan data.
 * @param data: Pointer to the received data buffer.
 * @retval None
 */
void parseYLIDARData(uint8_t *data)
{
    if (data[0] == YLIDAR_START_BYTE2 && data[1] == YLIDAR_START_BYTE1)
    {
        printf("Started parsing\r\n");

        // Extract fields in little-endian mode
        uint16_t packetHeader = (data[1] << 8) | data[0];
        uint8_t packageType = data[2] & 0x1;
        uint8_t scan_frequency = (data[1] >> 1)/10;
        uint8_t sampleQuantity = data[3];	// Number of 16 bits sample points
        uint16_t startAngleRaw = data[4] | (data[5] << 8);
		uint16_t endAngleRaw = data[6] | (data[7] << 8);
		uint16_t checksum = data[8] | (data[9] << 8);

        printf("Packet Header: 0x%X\r\n", packetHeader);
		printf("Package Type: %d\r\n", packageType);
		printf("Scan frequency: %d Hz\r\n", scan_frequency);
		printf("Sample Quantity: %d\r\n", sampleQuantity);

        // Calculate starting and ending angles
		float Angle_FSA = (startAngleRaw >> 1) / 64.0f; // Formula: Rshiftbit(FSA) / 64
		float Angle_LSA = (endAngleRaw >> 1) / 64.0f;   // Formula: Rshiftbit(LSA) / 64
		printf("Start Angle: %.2f°, End Angle: %.2f°\r\n", Angle_FSA, Angle_LSA);

		// Calculate the angle difference
		float diffAngle = (Angle_LSA > Angle_FSA) ? (Angle_LSA - Angle_FSA) : (360.0f + Angle_LSA - Angle_FSA);

		// Verify checksum
		uint16_t calculatedChecksum = 0;
		for (int i = 0; i <= YLIDAR_SAMPLE_BYTE_OFFSET; i+=2)
		{
			calculatedChecksum ^= data[i] | (data[i+1] << 8);
		}
		if (calculatedChecksum != checksum)
		{
			printf("Checksum mismatch! Calculated: 0x%X, Received: 0x%X\r\n", calculatedChecksum, checksum);
			return;
		}

		// Process sample data
		printf("Sample Data:\r\n");
		for (int i = 0; i < sampleQuantity; i++)
		{
			uint16_t sample = data[10 + (i * 2)] | (data[11 + (i * 2)] << 8);
			uint16_t distance = sample & 0xFFF; // Distance is lower 12 bits
			uint8_t interferenceFlag = (sample >> 12) & 0xF; // Upper 4 bits

			// Compute the intermediate angle
			float Angle_i = diffAngle * (i / (float)(sampleQuantity - 1)) + Angle_FSA;

			// Compute angle correction
			float AngCorrect = 0.0f;
			if (distance > 0)
			{
				AngCorrect = atan2f(21.8f * (155.3f - distance), 155.3f * distance) * (180.0f / PI);
			}
			float Corrected_Angle = Angle_i + AngCorrect;

			printf("Sample %d: Distance = %d mm, Interference = %d, Corrected Angle = %.2f°\r\n",
				   i + 1, distance, interferenceFlag, Corrected_Angle);
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
		// Add the received byte to the buffer
		if (bufferIndex < USART_BUFFER_SIZE)
		{
			uartBuffer[bufferIndex++] = rxByte;
		}
		else
		{
			printf("Buffer overflow! Clearing buffer.\r\n");
			bufferIndex = 0; // Reset buffer to prevent overflow
		}
		printf("Received byte: 0x%X\r\n", rxByte);

		// Synchronize to start bytes (0x55 0xAA)
		if (bufferIndex >= 2 &&
			uartBuffer[0] != YLIDAR_START_BYTE2 &&
			uartBuffer[1] != YLIDAR_START_BYTE1)
		{
			// Shift buffer to discard invalid start bytes
			memmove(uartBuffer, uartBuffer + 1, --bufferIndex);
		}

		// Process message if start bytes + full response (26 bytes) received
		if (bufferIndex >= 26 &&	// Minimum packet size (26 bytes for YDLIDAR)
			uartBuffer[bufferIndex-1] != YLIDAR_START_BYTE2 &&
			uartBuffer[bufferIndex] != YLIDAR_START_BYTE1)
		{
			parseYLIDARData(uartBuffer);

			// Reset the buffer for the next frame
			bufferIndex = 0;
			memset(uartBuffer, 0, sizeof(uartBuffer));
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
