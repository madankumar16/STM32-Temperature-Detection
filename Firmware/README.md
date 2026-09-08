# Firmware

The firmware is organized into small drivers:

- `STM32_Project/Core/Src/main.c` — application loop, STM32 initialization, alarm logic
- `STM32_Project/Drivers/LM35/lm35.c/.h` — ADC sampling and LM35 conversion
- `STM32_Project/Drivers/LCD/lcd.c/.h` — HD44780 4-bit LCD driver
- `STM32_Project/Drivers/Buzzer/buzzer.c/.h` — buzzer control
- `STM32_Project/Drivers/LED/led.c/.h` — alarm LED control

The source assumes the STM32Cube HAL package for STM32F1 is installed by the STM32CubeIDE project.
