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
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include "stm32g4xx_hal.h"

// Drivers
#include "../drivers/driver_Motor.h"
#include "../drivers/driver_GP2Y0A41SK0F.h"
#include "../drivers/driver_YLIDARX2.h"
#include "../drivers/driver_ADXL343.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEBUG 1 /**< Enable or disable debug logs */

#if DEBUG
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
// Messages
char small_logo_msg[] ="\r\n"
		"       \\_______/\r\n"
		"   `.,-'\\_____/`-.,'\r\n"
		"    /`..'\\ _ /`.,'\\\r\n"
		"   /  /`.,' `.,'\\  \\\r\n"
		"__/__/__/V-NOM\\__\\__\\__\r\n"
		"  \\  \\  \\     /  /  /\r\n"
		"   \\  \\,'`._,'`./  /\r\n"
		"    \\,'`./___\\,'`./\r\n"
		"   ,'`-./_____\\,-'`.\r\n"
		"       /       \\ \r\n";

char big_logo_msg[] = "\r\n"
		"⠀⠀⠀⠀⠀⠀⠀⠀⢀⣀⣤⠤⠶⠶⠶⠤⠤⣤⣀\t\r\n"
		"⠀⠀⠀⠀⢀⣠⠶⠛⠉⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠙⠳⢦⣀\t\r\n"
		"⠀⠀⠀⣰⠟⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⢷⡄\t\r\n"
		"⠀⠀⣰⠏⠀⢠⣶⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣤⡀⠀⠹⣆\t\r\n"
		"⠀⢠⡏⠀⢠⠟⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⢳⡄⠀⢹⡄\t\r\n"
		"⠀⣾⠀⠀⡞⠀⢸⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⠇⠀⢷⠀⠈⣇\t\r\n"
		"⢠⡇⠀⢠⠇⠀⠀⠙⣆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⠋⠀⠀⠸⡆⠀⣿\t\r\n"
		"⢸⡇⠀⢸⠀⠀⠀⠀⠙⢦⡀⠀⠀⠀⠀⠀⠀⢀⡾⠃⠀⠀⠀⢰⠇⠀⣿\t\r\n"
		"⢸⡇⠀⢸⠀⠀⠀⠀⠀⠀⠳⣄⣰⡄⢰⣄⣰⠋⠀⠀⠀⠀⠀⢸⡀⠀⣿\t\r\n"
		"⠘⡇⠀⢸⠀⠀⠀⠀⠀⠀⠀⠈⢹⡇⢸⠉⠁⠀⠀⠀⠀⠀⠀⠈⡇⠀⡟\t\r\n"
		"⠀⢿⠀⠈⢧⡀⠀⠀⠀⠀⠀⠀⣾⠀⢸⣄⠀⠀⠀⠀⠀⠀⠀⡼⢁⢸⡇\t\r\n"
		"⠀⢸⠘⣆⠈⢧⠀⠀⠀⠀⣠⠴⠃⠀⠀⠙⢧⣀⠀⠀⠀⠀⡼⠀⡼⢸⡃⠀⠀⠀⠀⠀⢀\t\r\n"
		"⠀⣾⠀⢸⣆⠘⠷⠤⠴⠚⣁⣀⣤⣤⣤⣄⣀⠉⠙⠲⠴⠚⢡⣾⠇⠘⡇⠀⠀⠀⠀⠀⠈⢷⣤⡀\t\r\n"
		"⠀⢹⣆⠈⣿⣷⣾⡖⣾⠻⣟⣾⣏⣻⣹⡟⡟⣿⢳⣶⣶⣾⣿⡟⠀⢰⡇⠀⠀⠀⠀⠀⠀⠘⣇⢻⡄\t\r\n"
		"⠀⠀⠻⣄⢸⣿⡋⠹⠿⣿⣿⡿⢿⣿⡟⣿⢿⣿⡟⠹⠋⠙⡿⠁⣴⠟⠀⠀⠀⠀⠀⠀⠀⠀⣿⠀⣷\t\r\n"
		"⠀⠀⠀⠹⣶⣿⣿⣄⠀⢻⣘⣦⣼⣿⣴⣿⣼⣼⣇⡀⣰⣿⠃⣴⠏⠀⠀⠀⠀⠀⠀⠀⢀⣴⠏⢀⡏\t\r\n"
		"⠀⠀⠀⠀⠙⣿⢿⣿⣥⣾⠛⠉⠀⠀⠉⠙⢦⡀⠈⠙⢿⣇⣰⠃⠀⠀⠀⠀⣀⣀⡤⠶⠛⠁⢠⡾⠁\t\r\n"
		"⠀⠀⠀⠀⠀⠘⣿⠻⣿⢻⣦⠀⢀⡀⠀⠀⠀⠙⠀⠀⠀⠹⣇⠀⠀⢀⣴⠟⠉⠀⠀⢀⣠⡼⠋\t\r\n"
		"⠀⠀⠀⠀⠀⠀⠘⣿⢹⡾⢻⣇⣴⡿⠳⢦⣀⠀⠀⠀⠀⠀⠙⠳⠶⠛⠁⠀⠀⢀⡼⠋⠁\t\r\n"
		"⠀⠀⠀⠀⠀⠀⠀⠘⣿⠻⡎⢹⢿⣷⣶⣎⣿⣧⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡾⠁\t\r\n"
		"⠀⠀⠀⠀⠀⠀⠀⠀⠸⣆⠙⢾⣸⠙⡿⢿⣩⠏⢹⣦⣀⠀⠀⠀⠀⠀⢀⣴⠟⠁\t\r\n"
		"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠹⣦⡀⠉⠉⠉⠉⠁⣰⠏⠀⠈⠉⠛⠒⠒⠛⠉⠁\t\r\n"
		"⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠉⠓⠒⠒⠒⠋⠁\t\r\n";

char jumbo_logo_msg[] = "\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⠿⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⠻⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⠟⠁⢠⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀⠙⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⠃⠀⢀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⠀⠀⠀⠈⢻⣿⣿⣿\r\n"
		"⣿⣿⣟⠁⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠇⢀⡀⠀⠀⠈⢿⣿⣿\r\n"
		"⣿⣿⡏⠀⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣟⣼⣿⠀⠀⠀⠈⣿⣿\r\n"
		"⣿⣿⠀⠀⠀⠀⠸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡏⠀⠀⠀⠀⢹⣿\r\n"
		"⣿⡿⠀⠀⠀⠀⠀⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠁⠀⠀⠀⠀⠈⣿\r\n"
		"⣿⡇⠀⠀⠀⠀⠀⠈⠉⠉⣻⣿⡿⣿⣿⣿⣿⢷⣺⣿⣿⣿⣿⣿⣿⣿⣿⣭⣭⣙⣿⠋⠁⠀⠀⠀⠀⠀⠀⣿\r\n"
		"⣿⡷⡶⠀⠀⠀⠀⠀⠀⠈⠋⠛⠻⠿⠿⣿⡟⢿⣿⣿⣿⣿⡿⠿⡿⡿⣿⣻⣿⡿⠁⠀⠀⠀⠀⠀⠀⠀⢸⣿\r\n"
		"⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠰⠦⠐⠛⢻⣿⣿⣿⣿⣿⣇⣼⣿⣿⣿⣿⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿\r\n"
		"⣿⣿⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣿⣯⣽⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿\r\n"
		"⣿⣿⣇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⡟⢿⣿⣿⣿⣿⣿⣿⣿⢃⣿⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣿⣿\r\n"
		"⣿⣿⣿⡇⢀⣀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠻⣮⣿⣿⣿⣿⣿⣿⡟⠿⠋⠀⠀⠀⠀⠀⠀⠀⠀⡀⠀⠀⣾⣿⣿\r\n"
		"⣿⣿⣿⡗⠈⣿⣿⣆⠀⠀⠀⠀⠀⠀⠀⠀⢀⡸⣿⣿⣿⣿⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⣾⣧⡄⢀⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣤⣿⣿⣿⣷⣄⣀⣴⣶⣶⣶⣤⣄⣹⣿⣿⣿⣿⠀⠀⢀⣀⠶⣄⣀⣤⡶⠾⢿⣿⣿⣷⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⢺⣷⢿⣿⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣯⣿⠛⠃⣿⢹⢷⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣾⣿⣾⣿⣿⣿⡿⣿⢿⠛⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣧⡇⡈⢹⣿⠉⢿⠟⢻⡿⠿⣿⠯⢿⣿⣿⣿⣿⣿⣿⠛⢿⠁⢻⣾⡀⣉⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣧⣧⢸⢸⠀⢸⠀⠀⠁⠀⢿⠀⢸⣿⠏⢹⡟⠀⣿⠀⢸⠀⣿⢹⣄⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⢸⡄⠘⠀⠀⠀⠀⣼⠀⠀⣿⠀⢨⡀⠀⢿⠀⢸⠀⣿⠘⢻⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⢸⡀⢸⡄⠀⣧⠀⢘⣿⠀⠀⣧⠀⢸⠀⣸⣸⣿⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⢿⣿⣿⣷⣸⣧⠀⣿⠀⢸⣿⡇⢠⡿⠀⣼⣻⣿⣿⠉⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⢸⣿⣿⣿⣿⣿⡆⣿⣆⢸⣿⡇⣼⡇⣸⣿⣿⣿⡟⠛⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣄⠀⣹⣿⣿⣿⣿⣿⣿⣿⣾⣿⣷⣿⣇⣿⣿⣿⣿⣿⣤⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⠀⠛⣿⣿⣿⣿⣿⣿⣿⣿⣿⣇⣿⣿⣿⣿⣿⡿⠙⠉⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⠀⢀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣏⣷⠀⢸⢻⣿⢹⣿⣿⣿⢻⣿⣿⣿⣿⣿⣿⣿⠙⠀⢘⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⡏⠀⢿⢸⣿⣿⡟⢸⣿⣿⣿⡇⣿⣿⠹⠀⢠⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣀⣸⠸⡏⢿⣇⠀⡙⠋⣿⡇⢸⡏⡇⠀⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⣿⡟⢹⠀⠁⠸⡇⠀⣤⠀⢿⠀⠸⠁⣆⣠⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣿⣷⣾⣄⣷⠀⣷⠀⣿⠀⢸⡇⣆⣰⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⣿⣤⣿⣧⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n";

// Global YLIDAR X2 handler
YLIDARX2_t hlidar;

// Global Motors handler
h_Motor_t hMotors;

// Global ToF sensors handler
h_GP2Y0A41SK0F_t hTof;

// Global ADXL343 (Accelerometer) handler
h_ADXL343_t hADXL;

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
	HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, HAL_MAX_DELAY);

	return ch;
}

/**
 * @brief UART receive half complete callback.
 * @param huart: Pointer to the UART handle
 * @retval None
 */
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART2) {
		YLIDARX2_ProcessDMAHalfComplete(&hlidar);
	}
}

/**
 * @brief UART receive complete callback.
 * @param huart: Pointer to the UART handle
 * @retval None
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART2) {
		YLIDARX2_ProcessDMAComplete(&hlidar);
	}
}

/**
 * @brief UART error callback.
 * @param huart: Pointer to the UART handle
 * @retval None
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART2)
	{
		if (HAL_UART_GetError(huart) & HAL_UART_ERROR_PE)
			DEBUG_PRINT("Parity Error!\r\n");
		if (HAL_UART_GetError(huart) & HAL_UART_ERROR_NE)
			DEBUG_PRINT("Noise Error!\r\n");
		if (HAL_UART_GetError(huart) & HAL_UART_ERROR_FE)
			DEBUG_PRINT("Framing Error!\r\n");
		if (HAL_UART_GetError(huart) & HAL_UART_ERROR_ORE)
			DEBUG_PRINT("Overrun Error!\r\n");
	}
}

void test_Motors(void)
{
	Motor_Init(&hMotors, &htim1);

	hMotors.mode_mot1 = FORWARD_MODE;
	hMotors.mode_mot2 = FORWARD_MODE;
	Motor_SetMode(&hMotors);
	Motor_SetSpeed_percent(&hMotors, 90.0, 90.0);
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
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
	printf("\r\n*** Waking up V-NOM ***\r\n");
	printf("%s", jumbo_logo_msg);

	/* Motors test & initialization *
	test_Motors();
	 */

	/* YLIDAR X2 Initialization with DMA */
	YLIDARX2_InitDMA(&hlidar, &huart2);

	/* ToF sensors Initialization *
	printf("GP2Y0A41SK0F Initialization...\r\n");
	GP2Y0A41SK0F_Init(&hTof);
	printf("GP2Y0A41SK0F Initialization Successful!\r\n");
	 */

	/* ADXL343 Initialization */
	//ADXL343_Init(&hADXL);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
		/* ToF test *
		GP2Y0A41SK0F_get_distance(&hTof);
		printf("ToF1 distance: %d mm, ToF2 distance: %d mm\r\n", hTof.distance_tof1, hTof.distance_tof2);

		/* Motors test *
		Motor_SetSpeed_percent(&hMotors, hTof.distance_tof1, hTof.distance_tof1);
		//printf("Mot1 speed: %d, Mot2 speed: %d\r\n", hMotors.current_speed1, hMotors.current_speed2);
		Motor_UpdateSpeed(&hMotors);

		/* ADXL343 test *
		ADXL343_get_Acceleration(&hADXL);
		 */
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
