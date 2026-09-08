# STM32 Temperature Detection System

STM32F103C8T6 (Blue Pill) based temperature monitoring system using an LM35 analog temperature sensor, 16x2 LCD, buzzer, and high-temperature LED alarm.

## Features
- LM35 temperature sensing through ADC1 channel 0 (PA0)
- Real-time temperature display on 16x2 HD44780 LCD
- High-temperature alarm using buzzer and red LED
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
1. Open the project in STM32CubeIDE.
2. Set up ADC1 on PA0 as a single regular conversion, 12-bit, software triggered.
3. Configure PB0/PB1 as GPIO outputs.
4. Configure LCD GPIO pins as push-pull outputs.
5. Build and flash with ST-Link.

> The `.ioc` file is a configuration reference. CubeMX-generated startup and HAL source files are intentionally not duplicated here.
