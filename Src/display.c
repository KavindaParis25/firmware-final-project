/*
 * acquisition.c
 *
 *  Created on: May 18, 2026
 *      Author: max
 */

#include "main.h"
#include "cmsis_os.h"
#include "io.h"

// Declaration of input queue, defined in main.c

extern osMessageQueueId_t displayQueueHandle;

void StartDisplayTask( void *argument ) {
  /* Infinite loop */
  for( ; ; ) {
	  // Variable to store the temperature value received from the queue.
	  uint16_t result;

	  // Wait for a temperature value from the default task and store it in result
	  osMessageQueueGet(displayQueueHandle,&result,NULL,osWaitForever);
	  //breaking the result into 4 digits
	  int d0 = (result/1000) %10;
	  int d1 = (result/100) %10;
	  int d2 = (result/10) %10;
	  // The last digit will represent the decimal place
	  int d3 = (result/1) %10;

	  // Write each digit to its LCD position
	  // '0'+ converts number to character
	  lcdWriteDigit('0'+d0,0);
	  lcdWriteDigit('0'+d1,1);
	  lcdWriteDigit('0'+d2,2);
	  lcdWriteDigit('0'+d3,3);

	  // Refresh the LCD screen to show the newly written digits
	  lcdUpdateDisplay();
  }
}

