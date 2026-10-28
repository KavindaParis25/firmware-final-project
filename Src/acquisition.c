/*
 * acquisition.c
 *
 *  Created on: May 18, 2026
 *      Author: max
 */

#include "main.h"
#include "cmsis_os.h"
#include <math.h>

// Declaration of output queue, defined in main.c

extern osMessageQueueId_t acquisitionQueueHandle;
extern osEventFlagsId_t acqEventHandle;
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;


// Acquisition task entry point

void StartAcquisitionTask( void *argument ) {
  /* Infinite loop */
  for( ; ; ) {
	// Sleep until timer interrupt signals a new acquisition is required
    osEventFlagsWait(acqEventHandle,0x01,osFlagsWaitAny,osWaitForever);
    // Start ADC1 conversion (reads voltage from NTC resistor divider)
    HAL_ADC_Start(&hadc1);
    // Wait until ADC1 conversion is complete
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    // Read the raw 12-bit digital signal from ADC1
    uint32_t adc1 = HAL_ADC_GetValue(&hadc1);
    // Stop ADC1 conversion to allow next triggered acquisition
    HAL_ADC_Stop(&hadc1);

    // Start ADC2 conversion (reads voltage from fixed resistor divider for Vcc measurement)
    HAL_ADC_Start(&hadc2);
    HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);
    uint32_t adc2 = HAL_ADC_GetValue(&hadc2);
    HAL_ADC_Stop(&hadc2);

    // Calculations necessary to determine the temperature
    float R_ntc = 10000.0f * adc1/(adc2 * 2.0f - adc1);
    float tempC = 25.0f + log(R_ntc/10000.0f)/log(0.96f);
    float tempK = tempC + 273.15f;

    //Result is stored with 1 digit for decimal place
    uint16_t result = (int32_t) (tempK * 10 + 0.5f);

    //The result obtained is put in the queue for the display
    osMessageQueuePut(acquisitionQueueHandle, &result,0,0);
  }
}

