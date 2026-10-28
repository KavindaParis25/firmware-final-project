/*
 * mem.h
 *
 * API for accessing the M24M01-DFMN6TP serial EEPROM
 * over the I2C3 peripheral at 100kHz.
 */

#ifndef INC_MEM_H_
#define INC_MEM_H_

#include <stdint.h>

// Initialize the serial memory subsystem.
// Verifies the EEPROM is reachable on the I2C bus.
// Returns 0 on success, -1 if the device does not respond.
int memInit(void);

// Read a single byte from the specified memory address.
// The result is written into the location pointed to by dataPtr.
// Returns 0 on success, -1 on I2C error.
int memRead(uint16_t address, uint8_t *dataPtr);

// Write a single byte (data) to the specified memory address.
// Waits for the EEPROM internal write cycle to complete before returning.
// Returns 0 on success, -1 on I2C error.
int memWrite(uint16_t address, uint8_t data);

#endif /* INC_MEM_H_ */
