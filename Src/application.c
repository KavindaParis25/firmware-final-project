/*
 * application.c
 *
 *  Created on: Oct 20, 2020
 *      Author: max
 */

#include "main.h"
#include "cmsis_os2.h"
#include "io.h"
#include "mem.h"


// Event flags object used to signal the acquisition task from the timer callback
osEventFlagsId_t acqEventHandle;


void StartDefaultTask( void *argument ) {

	// Create the event flag object so the acquisition task can block safely
    acqEventHandle = osEventFlagsNew(NULL);


    // ── EEPROM API Test ────────────────────────────────────────────────────

    // Test addresses -- spread across the memory space, none are zero
    const uint16_t addresses[] = { 0x0010, 0x00FF, 0x0100, 0x1000, 0x1234 };
    // Test data -- all values differ from 0x00 and 0xFF as required
    const uint8_t  testData[]  = { 0x42,   0xA5,   0x7E,   0x5A,   0x3C   };
    const int      N_TESTS     = 5;
    uint8_t        readBack;
    int            errors      = 0;

    // Step 1: Verify the EEPROM is reachable on the I2C bus
    if (memInit() != 0) {
        // Rapid LED0 blink signals that the EEPROM was not found
        while (1) {
            led0(ON);  osDelay(100);
            led0(OFF); osDelay(100);
        }
    }

    // Step 2: Write each test byte to its corresponding address
    for (int i = 0; i < N_TESTS; i++) {
        if (memWrite(addresses[i], testData[i]) != 0) {
            errors++;
        }
    }

    // Step 3: Read back each address and compare against the written value
    for (int i = 0; i < N_TESTS; i++) {
        if (memRead(addresses[i], &readBack) != 0) {
            errors++;                      // read operation failed
        } else if (readBack != testData[i]) {
            errors++;                      // data mismatch -- write or read error
        }
    }

    // Step 4: Signal overall result using LEDs
    if (errors == 0) {
        // All tests passed -- keep writing and reading so oscilloscope can capture
        while (1) {
            memWrite(0x0010, 0x42);
            memRead(0x0010, &readBack);

            // Blink LEDs to confirm pass
            led0(ON); led1(ON); led2(ON); led3(ON);
            osDelay(500);
            led0(OFF); led1(OFF); led2(OFF); led3(OFF);
            osDelay(500);
        }
    } else {
        // One or more tests failed -- LED0 stays solid
        led0(ON);
        while (1) { osDelay(1000); }
    }
}
