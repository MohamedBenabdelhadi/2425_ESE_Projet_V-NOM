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
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠁⢠⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀⠙⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠃⠀⢀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⠀⠀⠀⠈⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣟⠁⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠇⢀⡀⠀⠀⠈⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡏⠀⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣟⣼⣿⠀⠀⠀⠈⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⠀⠀⠸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡏⠀⠀⠀⠀⢹⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠀⠀⠀⠀⠀⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠁⠀⠀⠀⠀⠈⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀⠀⠀⠀⠈⠉⠉⣻⣿⡿⣿⣿⣿⣿⢷⣺⣿⣿⣿⣿⣿⣿⣿⣿⣭⣭⣙⣿⠋⠁⠀⠀⠀⠀⠀⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡷⡶⠀⠀⠀⠀⠀⠀⠈⠋⠛⠻⠿⠿⣿⡟⢿⣿⣿⣿⣿⡿⠿⡿⡿⣿⣻⣿⡿⠁⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠰⠦⠐⠛⢻⣿⣿⣿⣿⣿⣇⣼⣿⣿⣿⣿⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⣿⣯⣽⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⡟⢿⣿⣿⣿⣿⣿⣿⣿⢃⣿⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⢀⣀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠻⣮⣿⣿⣿⣿⣿⣿⡟⠿⠋⠀⠀⠀⠀⠀⠀⠀⠀⡀⠀⠀⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡗⠈⣿⣿⣆⠀⠀⠀⠀⠀⠀⠀⠀⢀⡸⣿⣿⣿⣿⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⣾⣧⡄⢀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣤⣿⣿⣿⣷⣄⣀⣴⣶⣶⣶⣤⣄⣹⣿⣿⣿⣿⠀⠀⢀⣀⠶⣄⣀⣤⡶⠾⢿⣿⣿⣷⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⢺⣷⢿⣿⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣯⣿⠛⠃⣿⢹⢷⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣾⣿⣾⣿⣿⣿⡿⣿⢿⠛⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⡇⡈⢹⣿⠉⢿⠟⢻⡿⠿⣿⠯⢿⣿⣿⣿⣿⣿⣿⠛⢿⠁⢻⣾⡀⣉⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⣧⢸⢸⠀⢸⠀⠀⠁⠀⢿⠀⢸⣿⠏⢹⡟⠀⣿⠀⢸⠀⣿⢹⣄⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⢸⡄⠘⠀⠀⠀⠀⣼⠀⠀⣿⠀⢨⡀⠀⢿⠀⢸⠀⣿⠘⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⢸⡀⢸⡄⠀⣧⠀⢘⣿⠀⠀⣧⠀⢸⠀⣸⣸⣿⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⢿⣿⣿⣷⣸⣧⠀⣿⠀⢸⣿⡇⢠⡿⠀⣼⣻⣿⣿⠉⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⢸⣿⣿⣿⣿⣿⡆⣿⣆⢸⣿⡇⣼⡇⣸⣿⣿⣿⡟⠛⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣄⠀⣹⣿⣿⣿⣿⣿⣿⣿⣾⣿⣷⣿⣇⣿⣿⣿⣿⣿⣤⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⠀⠛⣿⣿⣿⣿⣿⣿⣿⣿⣿⣇⣿⣿⣿⣿⣿⡿⠙⠉⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⠀⢀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣏⣷⠀⢸⢻⣿⢹⣿⣿⣿⢻⣿⣿⣿⣿⣿⣿⣿⠙⠀⢘⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⡏⠀⢿⢸⣿⣿⡟⢸⣿⣿⣿⡇⣿⣿⠹⠀⢠⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣀⣸⠸⡏⢿⣇⠀⡙⠋⣿⡇⢸⡏⡇⠀⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⣿⡟⢹⠀⠁⠸⡇⠀⣤⠀⢿⠀⠸⠁⣆⣠⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣿⣷⣾⣄⣷⠀⣷⠀⣿⠀⢸⡇⣆⣰⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⣿⣤⣿⣧⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n"
		"⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\r\n";

// YLIDAR X2
uint8_t rxByte; // For single-byte reception
h_YLIDARX2_t hYLIDAR;

// Motors
h_Motor_t hMotor;

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
 * @brief UART receive complete callback.
 * @param huart: Pointer to the UART handle
 * @retval None
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART2)
	{
		printf("UART2: 0x%X\r\n", rxByte);
		YLIDARX2_UART_irq(&hYLIDAR);

		// Restart reception for the next byte
		HAL_UART_Receive_IT(&huart2, &rxByte, 1);
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
			printf("Parity Error!\r\n");
		if (HAL_UART_GetError(huart) & HAL_UART_ERROR_NE)
			printf("Noise Error!\r\n");
		if (HAL_UART_GetError(huart) & HAL_UART_ERROR_FE)
			printf("Framing Error!\r\n");
		if (HAL_UART_GetError(huart) & HAL_UART_ERROR_ORE)
			printf("Overrun Error!\r\n");

		// Restart UART reception after error
		HAL_UART_Receive_IT(&huart2, &rxByte, 1);
	}
}

void init_ADXL(void)
{
	printf("\r\nInitializing ADXL...\r\n");

	// Initialize ADXL configuration structure
	ADXL_InitTypeDef adxlConfig;

	adxlConfig.LPMode = LPMODE_NORMAL;
	adxlConfig.Rate = 10; // 100 Hz
	adxlConfig.SPIMode = 0; // 4-wire SPI
	adxlConfig.IntMode = 0; // Active High
	adxlConfig.Justify = 0; // Signed
	adxlConfig.Resolution = RESOLUTION_10BIT;
	adxlConfig.Range = RANGE_2G;
	adxlConfig.AutoSleep = AUTOSLEEPOFF;
	adxlConfig.LinkMode = LINKMODEOFF;

	// Initialize ADXL
	if (ADXL_Init(&adxlConfig) != ADXL_OK)
	{
		printf("ADXL Initialization Failed!\r\n");
		while (1);
	}

	printf("ADXL Initialization Successful!\r\n");

	// Start Measurement Mode
	ADXL_Measure(ON);

	// Data array for accelerometer readings
	int16_t accData[3] = {0, 0, 0};

	printf("Reading accelerometer data:\r\n");

	// Start a 100ms timer
	uint32_t startTime = HAL_GetTick();
	while (HAL_GetTick() - startTime < 100)
	{
		ADXL_getAccel(accData, OUTPUT_SIGNED);
		printf("X: %d, Y: %d, Z: %d\r\n", accData[0], accData[1], accData[2]);
		HAL_Delay(10); // Delay for 10ms between readings
	}

	printf("Finished reading accelerometer data.\r\n");

	// Put ADXL in standby mode
	ADXL_Standby(ON);
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
	MX_SPI1_Init();
	MX_TIM1_Init();
	MX_TIM3_Init();
	MX_TIM4_Init();
	MX_USART2_UART_Init();
	MX_USART1_UART_Init();
	MX_ADC1_Init();
	/* USER CODE BEGIN 2 */
	printf("\r\n*** Waking up V-NOM ***\r\n");
	printf("%s", big_logo_msg);

	// Motor initialization
	Motor_Init(&hMotor, &htim1);

	// YLIDAR X2 Initialization
	printf("YLIDAR X2 Initialization...\r\n");
	hYLIDAR.uart_buffer = &rxByte;
	// Start UART reception in interrupt mode (1 byte at a time)
	HAL_UART_Receive_IT(&huart2, &rxByte, 1);
	printf("YLIDAR X2 Initialization Successful!\r\n");

	// ADXL343 Initialization
	//init_ADXL();
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		Motor_UpdateSpeed(&hMotor);
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
