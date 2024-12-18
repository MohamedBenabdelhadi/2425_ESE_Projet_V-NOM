/*
 * driver_YLIDARX2.c
 *
 *  Created on: Dec 11, 2024
 *      Author: oliver
 */

#include "driver_YLIDARX2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define LOGS 0
#define DEBUG 1

uint16_t bufferIndex = 0;
uint8_t uartBuffer[USART_BUFFER_SIZE];


/**
 * Debugging function to print data
 */
void YLIDARX2_PrintData(h_YLIDARX2_t * hYLIDAR)
{
#if (LOGS)
		// Extract fields in little-endian mode
		uint16_t packetHeader = (hYLIDAR->data_buffer[1] << 8) | hYLIDAR->data_buffer[0];
		uint8_t packageType = hYLIDAR->data_buffer[2] & 0x1;
		uint8_t scan_frequency = (hYLIDAR->data_buffer[2] >> 1)/10;

		printf("Packet Header: 0x%X\r\n", packetHeader);
		printf("Package Type: %s\r\n", YLIDAR_PACKAGE_TYPE(packageType));
		printf("Scan frequency: %d Hz\r\n", scan_frequency);
#endif
		printf("Sample Quantity: %d\r\n", hYLIDAR->sample_quantity);

	printf("Data: ");

	for (int i=0; i < hYLIDAR->data_length; i++)
	{
		printf("0x%X ", hYLIDAR->data_buffer[i]);

		if (i%10 == 0) printf("\r\n");
	}

	printf("\r\n");
}

/**
 * Debugging function to print samples
 */
void YLIDARX2_PrintSamples(h_YLIDARX2_t * hYLIDAR)
{
	for (int i=0; i < hYLIDAR->sample_quantity; i++)
			{
				printf("Sample %d: Distance = %d mm, ", i + 1, hYLIDAR->samples[i].distance);
	#if (LOGS)
				printf("Interference = %d, ", hYLIDAR->samples[i].interference_flag);
	#endif
				printf("Corrected Angle = %.2f°\r\n", hYLIDAR->samples[i].corrected_angle);
			}
}

uint16_t YLIDARX2_CalculateChecksum(uint8_t *data, uint16_t length)
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
void YLIDARX2_ParseData(h_YLIDARX2_t * hYLIDAR)
{
	if (hYLIDAR->data_buffer[0] == YLIDAR_START_BYTE2 && hYLIDAR->data_buffer[1] == YLIDAR_START_BYTE1)
	{
#if (LOGS)
		printf("Started parsing\r\n");
#endif

		// Verify checksum
		uint16_t checksum = hYLIDAR->data_buffer[8] | (hYLIDAR->data_buffer[9] << 8);
		uint16_t calculatedChecksum = YLIDARX2_CalculateChecksum(hYLIDAR->data_buffer, YLIDAR_SAMPLE_BYTE_OFFSET);

		if (calculatedChecksum != checksum)
		{
#if (LOGS)
			printf("Checksum mismatch! Calculated: 0x%X, Received: 0x%X\r\n", calculatedChecksum, checksum);
#endif
			return;
		}
#if (DEBUG)
		YLIDARX2_PrintData(hYLIDAR);
#endif
		uint16_t startAngleRaw = hYLIDAR->data_buffer[4] | (hYLIDAR->data_buffer[5] << 8);
		uint16_t endAngleRaw = hYLIDAR->data_buffer[6] | (hYLIDAR->data_buffer[7] << 8);

		// Calculate starting and ending angles
		float Angle_FSA = (startAngleRaw >> 1) / 64.0f; // Formula: Rshiftbit(FSA) / 64
		float Angle_LSA = (endAngleRaw >> 1) / 64.0f;   // Formula: Rshiftbit(LSA) / 64
#if (LOGS)
		printf("Start Angle: %.2f°, End Angle: %.2f°\r\n", Angle_FSA, Angle_LSA);
#endif

		// Calculate the angle difference
		float diffAngle = (Angle_LSA > Angle_FSA) ? (Angle_LSA - Angle_FSA) : (360.0f + Angle_LSA - Angle_FSA);
#if (DEBUG)
		// Process sample data
		printf("Sample Data:\r\n");
#endif
		YLIDARX2_sample_t samples[hYLIDAR->sample_quantity];

		for (int i = 0; i < hYLIDAR->sample_quantity; i++)
		{
			samples[i].data = hYLIDAR->data_buffer[10 + i*2] | (hYLIDAR->data_buffer[11 + i*2] << 8);
			samples[i].distance = (uint16_t)((samples[i].data) >> 2);
			samples[i].interference_flag = (samples[i].data) & 0b11; // Lower 2 bits

			// Compute the intermediate angle
			float Angle_i = diffAngle * (float)((i - 1)/(hYLIDAR->sample_quantity-1)) + Angle_FSA;

			// Compute angle correction
			float AngCorrect = 0.0f;

			if (samples[i].distance > 0)
			{
				AngCorrect = atan(21.8f * (155.3f - samples[i].distance)/(155.3f * samples[i].distance) ) * (180.0f / PI);
			}

			samples[i].corrected_angle = Angle_i + AngCorrect;
		}

		hYLIDAR->samples = samples;
#if (DEBUG)
		YLIDARX2_PrintSamples(hYLIDAR);
#endif
	}
	else
	{
		printf("YLIDAR X2: Invalid start bytes!\r\n");
	}
}

/**
 * @param	UART buffer, should be a variable or an array of 1.
 */
void YLIDARX2_UART_irq(h_YLIDARX2_t * hYLIDAR)
{
	// Add received byte to the buffer
	uartBuffer[bufferIndex++] = hYLIDAR->uart_buffer[0];

	// Check for start bytes and process data only if a full packet is received
	if (bufferIndex >= 2)
	{
		if(uartBuffer[0] == YLIDAR_START_BYTE2 && uartBuffer[1] == YLIDAR_START_BYTE1)
		{
			if (bufferIndex >= YLIDAR_PACKET_HEADER_LENGTH) // Minimum packet size
			{
				// Extract sample quantity
				hYLIDAR->sample_quantity = uartBuffer[3];
				uint16_t expectedLength = YLIDAR_PACKET_HEADER_LENGTH + (hYLIDAR->sample_quantity * 2);

				// Process only when the full packet is received
				if (bufferIndex >= expectedLength)
				{
					hYLIDAR->data_buffer = uartBuffer;
					hYLIDAR->data_length = bufferIndex;

					YLIDARX2_ParseData(hYLIDAR);

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
		printf("YLIDAR X2: Buffer overflow! Clearing buffer.\r\n");
#endif
	}
}
