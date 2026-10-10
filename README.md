# STM32 Temperature Detection System

STM32F103C8T6 (Blue Pill) based temperature monitoring system using an LM35 analog temperature sensor, 16x2 LCD, buzzer, and high-temperature LED alarm.

## Features
- LM35 temperature sensing through ADC1 channel 0 (PA0)
- Real-time temperature display on 16x2 HD44780 LCD
- High-temperature alarm using buzzer and red LED
- Fail-safe buzzer/LED alert when an ADC read fails or the LM35 signal appears disconnected/shorted
- Modular C drivers for LM35, LCD, LED, and buzzer
- Designed for STM32CubeIDE / STM32 HAL

## Hardware
| Component | Purpose |
|---|---|
| STM32F103C8T6 Blue Pill | Main MCU |
| LM35 | Analog temperature sensor |
| 16x2 HD44780 LCD | Temperature display |
| 5V buzzer | Audible alarm |
| Red LED + 220 ohm resistor | Visual alarm |
| 10k potentiometer | LCD contrast |
| Breadboard + jumper wires | Assembly |
| ST-Link V2 | Programming/debugging |

## Pin Mapping
- LM35 VOUT -> PA0 / ADC1_IN0
- LCD RS -> PB12
- LCD E -> PB13
- LCD D4 -> PB14
- LCD D5 -> PB15
- LCD D6 -> PA8
- LCD D7 -> PA9
- Buzzer -> PB0
- Red LED -> PB1 through 220 ohm resistor
- LCD VSS -> GND
- LCD VDD -> 3.3V
- ST-Link SWDIO -> PA13
- ST-Link SWCLK -> PA14

## Temperature calculation
For a 12-bit ADC with 3.3 V reference:

`Vout = ADC * 3.3 / 4095`

For LM35:

`Temperature_C = Vout * 100`

## Alarm threshold
The example firmware uses **50.0 C** as the high-temperature threshold. Change `LM35_ALARM_THRESHOLD_C` in `lm35.h` to suit your application.

## Project structure
```text
Documentation/
  Blueprint.png
  Block_Diagram.md
  Circuit_Diagram.md
  Working_Principle.md
Hardware/
  Bill_of_Materials.md
  Pin_Connections.md
STM32_Project/
  Core/Inc/main.h
  Core/Src/main.c
  Drivers/LM35/lm35.c
  Drivers/LM35/lm35.h
  Drivers/LCD/lcd.c
  Drivers/LCD/lcd.h
  Drivers/Buzzer/buzzer.c
  Drivers/Buzzer/buzzer.h
  Drivers/LED/led.c
  Drivers/LED/led.h
  STM32_Temperature.ioc
Firmware/README.md
LICENSE
```

## Build
This repository contains the application C sources and a human-readable pin/configuration reference; it is not a complete CubeIDE-importable project by itself.

1. In STM32CubeIDE, create a project for **STM32F103C8T6** (the Blue Pill target) or open your existing generated project.
2. Configure ADC1 channel 0 on PA0 as a single, software-triggered 12-bit conversion.
3. Configure PB0/PB1 and the LCD pins as GPIO outputs, and keep PA13/PA14 available for SWD.
4. Add the files in `STM32_Project/Core/Src` and the `STM32_Project/Drivers/*` C/header folders to your generated project. Add the driver folders to the compiler include paths.
5. Use the `stm32f1xx_hal_conf.h` in `STM32_Project/Core/Inc` or merge its enabled module settings into your generated HAL configuration.
6. Build in STM32CubeIDE and flash with ST-Link. Verify sensor thresholds and wiring on the actual board before relying on the alarm.

> `STM32_Temperature.ioc` is a human-readable pin/configuration reference, not a CubeMX-generated `.ioc` file. The startup code, linker script, and vendor HAL sources are intentionally not vendored into this repository. GitHub Actions compile-checks the C sources against vendor headers, but does not link a firmware image or replace hardware testing.
