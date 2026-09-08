# Circuit Diagram

See [`Blueprint.svg`](./Blueprint.svg) for the visual system blueprint.

The electrical mapping is:

- LM35 output to PA0 (ADC1_IN0)
- LCD RS/E/D4/D5/D6/D7 to PB12/PB13/PB14/PB15/PA8/PA9
- Buzzer control to PB0
- Red LED through 220 ohm resistor to PB1
- LCD RW to GND
- LCD contrast pin V0 to a 10k potentiometer wiper
- Common ground between all modules

For modules requiring a different supply voltage, use proper level shifting/driver circuitry. The blueprint is a logical connection diagram, not a substitute for the component datasheets.
