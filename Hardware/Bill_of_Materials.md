# Bill of Materials

| # | Component | Qty | Notes |
|---|---|---:|---|
| 1 | STM32F103C8T6 Blue Pill | 1 | Main controller |
| 2 | LM35 | 1 | Analog temperature sensor |
| 3 | 16x2 HD44780 LCD | 1 | 4-bit interface |
| 4 | 10k potentiometer | 1 | LCD contrast |
| 5 | 5V active buzzer | 1 | High-temperature alarm |
| 6 | LED | 1 | Red alarm indicator |
| 7 | 220 ohm resistor | 1 | LED current limiting |
| 8 | Breadboard | 1 | Prototype assembly |
| 9 | Jumper wires | As required | Male/female as needed |
| 10 | ST-Link V2 | 1 | Programming/debugging |
| 11 | 3.3V regulated supply | 1 | MCU supply; check peripherals before powering |

## Safety / power note
The Blue Pill is a 3.3 V MCU board. Do not connect a 5 V signal directly to an STM32 GPIO. If the selected LCD or buzzer requires 5 V, verify its input thresholds and use suitable level shifting or a transistor driver where necessary.
