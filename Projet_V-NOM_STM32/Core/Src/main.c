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
#include "cmsis_os.h"
#include "adc.h"
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

#define STACK_SIZE 256
#define TASK_PRIORITY_MOTOR 1

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
h_YLIDARX2_t hlidar;

// Global Motors handler
h_Motor_t hMotors;

// Global ToF sensors handler
h_GP2Y0A41SK0F_t hTof;

// Global ADXL343 (Accelerometer) handler
h_ADXL343_t hADXL;

// Task handles
TaskHandle_t xHandle1;
TaskHandle_t xHandle2;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
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

		//YLIDARX2_RestartDMA(&hlidar);
	}
}

/**
 * @brief ADC conversion complete callback.
 * This function is called when an ADC conversion is complete.
 * It retrieves the ADC value and processes the data.
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	if (hadc->Instance == ADC1) {
		hTof.adc_val_tof1 = HAL_ADC_GetValue(hadc);
		GP2Y0A41SK0F_get_distance1(&hTof);
	}
	else if (hadc->Instance == ADC2) {
		hTof.adc_val_tof2 = HAL_ADC_GetValue(hadc);
		GP2Y0A41SK0F_get_distance2(&hTof);
	}
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc) {
	if (hadc->ErrorCode != HAL_ADC_ERROR_NONE) {
		printf("ADC Error Code: %lx\n", hadc->ErrorCode);

		GP2Y0A41SK0F_Start_Interrupt(&hTof);
	}
}

/**
 * @brief GPIO interrupt callback for EXTI line.
 * This function is triggered by the accelerometer interrupt.
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == AG_INT1_Pin) // Interrupt triggered by INT1
    {
        uint8_t int_source;
        ADXL343_readRegister(ADXL343_INT_SOURCE, &int_source, 1);

        if (int_source & 0x40) // Check SINGLE_TAP bit
        {
        	DEBUG_PRINT("Single Tap Detected!\n");
        	Motor_Stop(&hMotors);
        	vTaskDelete(xHandle2);
        }
    }
    else if (GPIO_Pin == AG_INT2_Pin) // Interrupt triggered by INT2
    {
    	DEBUG_PRINT("Interrupt triggered on INT2\n");
        // Handle other potential events from INT2 if needed
    }
}

/**
 * @brief  Gestion d'erreurs de création de tâche.
 * @param	BaseType_t	Retour de xTaskCreate.
 */
void errHandler_xTaskCreate(BaseType_t r)
{
	/* Vérification si la tâche a été créée avec succès */
	if (pdPASS == r) {
		/* Si la tâche est créée avec succès, démarrer le scheduler */
		DEBUG_PRINT("Tâche crée avec succès\r\n");
	} else if (errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY == r) {
		DEBUG_PRINT("Erreur: Mémoire insuffisante\r\n");
		Error_Handler();
	} else {
		/* Cas improbable : code d'erreur non prévu pour xTaskCreate */
		DEBUG_PRINT("Erreur inconnue lors de la création de la tâche\r\n");
		Error_Handler();  	// Gestion d'erreur générique
		NVIC_SystemReset(); // Réinitialiser le microcontrôleur
	}
}

#define TOF_TRESHHOLD 40

void task_Motors(void * unsused)
{
	/* Motors test */
	Motor_Init(&hMotors, &htim1);

	while (1)
	{
		DEBUG_PRINT("Mot1 speed: %d, Mot2 speed: %d\r\n", hMotors.current_speed1, hMotors.current_speed2);
		Motor_UpdateSpeed(&hMotors);
	}
}

void task_Behaviour(void * unsused)
{
	while (1)
	{
		DEBUG_PRINT("ToF1 distance: %d mm, ToF2 distance: %d mm\r\n", hTof.distance_tof1, hTof.distance_tof2);

		/* Motors test */
		if (hTof.distance_tof2 > TOF_TRESHHOLD && hTof.distance_tof1 > TOF_TRESHHOLD)
		{
			hMotors.mode_mot1 = FORWARD_MODE;
			hMotors.mode_mot2 = FORWARD_MODE;

			Motor_SetMode(&hMotors);
			Motor_SetSpeed_percent(&hMotors, 40, 40);
			vTaskDelay(1000 / portTICK_PERIOD_MS);
		}
		else if (hTof.distance_tof2 > TOF_TRESHHOLD && hTof.distance_tof1 <= TOF_TRESHHOLD)
		{
			hMotors.mode_mot1 = FORWARD_MODE;
			hMotors.mode_mot2 = REVERSE_MODE;
		}
		else if (hTof.distance_tof2 <= TOF_TRESHHOLD && hTof.distance_tof1 > TOF_TRESHHOLD)
		{
			hMotors.mode_mot1 = REVERSE_MODE;
			hMotors.mode_mot2 = FORWARD_MODE;
		}
		else //if (hTof.distance_tof2 <= TOF_TRESHHOLD && hTof.distance_tof1 <= TOF_TRESHHOLD)
		{
			hMotors.mode_mot1 = REVERSE_MODE;
			hMotors.mode_mot2 = REVERSE_MODE;
		}

		Motor_SetMode(&hMotors);
		Motor_SetSpeed_percent(&hMotors, 40, 40);

		vTaskDelay(1);
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
	BaseType_t xReturned;

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
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  /* USER CODE BEGIN 2 */
	printf("\r\n*** Waking up V-NOM ***\r\n");
	//printf("%s", jumbo_logo_msg);

	/* ToF sensors Initialization */
	GP2Y0A41SK0F_Init(&hTof);

	/* Motors task */
	xReturned = xTaskCreate(
			task_Motors, // Function that implements the task.
			"task_Motors", // Text name for the task.
			STACK_SIZE, // Stack size in words, not bytes.
			(void *) 0, // Parameter passed into the task.
			TASK_PRIORITY_MOTOR,// Priority at which the task is created.
			&xHandle1); // Used to pass out the created task's handle.

	errHandler_xTaskCreate(xReturned);

	/* Behaviour task */
	xReturned = xTaskCreate(
			task_Behaviour, // Function that implements the task.
			"task_Behaviour", // Text name for the task.
			STACK_SIZE, // Stack size in words, not bytes.
			(void *) 0, // Parameter passed into the task.
			TASK_PRIORITY_MOTOR,// Priority at which the task is created.
			&xHandle2); // Used to pass out the created task's handle.

	errHandler_xTaskCreate(xReturned);

	/* YLIDAR X2 Initialization with DMA *
	LIDAR_RX_GPIO_Port->PUPDR = GPIO_PULLUP;
	YLIDARX2_InitDMA(&hlidar, &huart2);*/

	/* ADXL343 Initialization *
	ADXL343_Init(&hADXL);*/

  /* USER CODE END 2 */

  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
		/* ADXL343 test *
		ADXL343_get_Acceleration(&hADXL);*/

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
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM2 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM2) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
