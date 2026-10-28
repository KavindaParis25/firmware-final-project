/*
 * heater.c
 *
 *  Created on: Oct 20, 2020
 *      Author: max
 */

#include "main.h"
#include "cmsis_os2.h"

// Heater task is not used in this project
// Stub kept so the build system resolves the symbol
void StartHeaterTask(void *argument) {
    for (;;) {
        osDelay(1000);
    }
}
