# Pin Connections

## LM35
| LM35 pin | Connect to |
|---|---|
| VCC | 3.3 V |
| VOUT | PA0 / ADC1_IN0 |
| GND | GND |

## LCD 16x2 in 4-bit mode
| LCD pin | STM32 |
|---|---|
| VSS | GND |
| VDD | 3.3 V (verify LCD module operating range) |
| V0 | Wiper of 10k potentiometer |
| RS | PB12 |
| RW | GND |
| E | PB13 |
| D4 | PB14 |
| D5 | PB15 |
| D6 | PA8 |
| D7 | PA9 |
| A/K | Backlight supply/ground per module datasheet |

## Alarm
| Device | STM32 pin |
|---|---|
| Buzzer control | PB0 |
| LED anode through 220 ohm | PB1 |
| LED cathode | GND |

## Programming
| ST-Link | Blue Pill |
|---|---|
| SWDIO | PA13 |
| SWCLK | PA14 |
| GND | GND |
| 3.3V | 3.3V |

The exact electrical interface of a particular LCD/buzzer module must be checked against its datasheet. For a higher-current buzzer, drive it through an NPN transistor/MOSFET with a suitable protection diode if required.
