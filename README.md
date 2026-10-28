# I²C Serial EEPROM Driver (M24M01) on STM32L496

Team final project for the *Introduction to Embedded Systems* course at Politecnico di Torino, A.Y. 2025/26 (June 2026).

The project implements a small software API to access an **M24M01-DFMN6TP** I²C EEPROM (128 KB / 1 Mbit) from an **STM32L496** microcontroller on the VirtLAB board. It is built on the FreeRTOS template from the previous labs and uses the STM32 HAL.

## Team 13

- Gulin Yildiz
- Mihindukulasuriya Amandi Thisara Fernando
- Kavinda Madushan Paris Mahipala Mudalige
- Basthiyan Koralalage Jude Hirusha Rodrigo

## API

Declared in `mem.h`, implemented in `mem.c`:

```c
int memInit(void);                                  // check the EEPROM is present on the bus
int memRead(uint16_t address, uint8_t *dataPtr);    // read one byte
int memWrite(uint16_t address, uint8_t data);       // write one byte (waits for the write cycle)
```

All functions return `0` on success and `-1` on error.

| Function | What it does |
|---|---|
| `memInit()` | Calls `HAL_I2C_IsDeviceReady()` on address `0xAC` (3 retries, 100 ms timeout) and returns `-1` if the EEPROM does not acknowledge. |
| `memWrite()` | Calls `HAL_I2C_Mem_Write()` with a 16-bit memory address, then `HAL_Delay(5)` to wait for the EEPROM internal write cycle (tW = 5 ms max). |
| `memRead()` | Calls `HAL_I2C_Mem_Read()`. The HAL sends a dummy write to set the address, then a repeated START and a read (`0xAD`). No delay is needed after a read. |

## Hardware

| Item | Details |
|---|---|
| MCU board | VirtLAB board, STM32L496 (Cortex-M4, up to 80 MHz), I²C master |
| EEPROM | M24M01-DFMN6TP on the X-NUCLEO-EEPRMA1 daughter board, I²C slave |
| Peripheral | I2C3, standard mode (100 kHz) |
| Pins | PC0 = I2C3_SCL, PC1 = I2C3_SDA |
| Pull-ups | 3.3 kΩ on SCL and SDA, already on the daughter board |
| Measurement | Rigol DS1054Z oscilloscope on the daughter board probe connector |

The VirtLAB jumpers must be set so the Arduino connector pins are routed to I2C3 (see the project sheet). Otherwise the EEPROM never acknowledges and `memInit()` fails.

### Device address

The device select code is `1010 E2 E1 A16 R/W`. On the daughter board E2 = E1 = 1 (tied to VCC), and all test addresses are below `0x10000`, so A16 = 0:

```
1010 1 1 0 0 = 0xAC   (write)      0xAD for a read
```

The M24M01 has 2^17 locations, so A16 travels inside the device select byte and the other 16 address bits are sent as two bytes (`I2C_MEMADD_SIZE_16BIT`).

## Test program

`StartDefaultTask` in `application.c` exercises the API:

1. `memInit()` checks the EEPROM. On failure, LED0 blinks fast forever.
2. Writes 5 bytes (`0x42, 0xA5, 0x7E, 0x5A, 0x3C`) to addresses `0x0010, 0x00FF, 0x0100, 0x1000, 0x1234`.
3. Reads them back and compares. Any failed read or mismatch increments an error counter.
4. If there are no errors, all four LEDs blink at 1 Hz and the task keeps writing and reading address `0x0010`, so the bus stays active for the oscilloscope. If there are errors, LED0 stays solid.

## Results

Oscilloscope captures of SDA/SCL confirmed:

- Correct **write** transaction: START, `0xAC`, address high, address low, data, each ACKed, then STOP.
- Correct **read** transaction: dummy write phase, **repeated START**, `0xAD`, data byte, master NACK, STOP.
- The idle gap of about 5 ms between a write and the next access, matching the EEPROM write-cycle time.
- Bus signal swing of about 3.3 V. The measured SCL frequency was about 70 kHz, lower than the configured 100 kHz.

All five write/read pairs matched in the test program.

## Repository structure

```
firmware-final-project/
├── Core/
│   ├── Inc/            # mem.h, main.h, ...
│   └── Src/            # mem.c, application.c, main.c (CubeMX generated)
├── Drivers/            # STM32 HAL / CMSIS
├── *.ioc               # STM32CubeMX configuration
└── README.md
```

> Adjust the folder layout to match the repository.

## Build and flash

1. Install **STM32CubeIDE**.
2. Clone the repository:
   ```bash
   git clone https://github.com/<your-username>/firmware-final-project.git
   ```
3. In STM32CubeIDE: *File → Open Projects from File System…* and select the cloned folder.
4. Build with *Project → Build All*.
5. Connect the VirtLAB board with the daughter board mounted and flash with *Run*.

## Documentation

The full project report (flowcharts, oscilloscope captures and annotated source code) is the FW_FINAL_PROJECT.pdf, June 2026.

## License

Coursework project. All rights reserved by the authors, unless a license is added.
