# Working Principle

1. The LM35 produces an analog voltage proportional to temperature, approximately 10 mV per degree Celsius.
2. STM32 ADC1 samples the sensor voltage on PA0 using a 12-bit conversion. The ADC is calibrated once at power-up, and each reading is the average of 16 conversions.
3. Firmware converts the ADC count to voltage using a 3.3 V reference assumption.
4. The voltage is multiplied by 100 to obtain temperature in Celsius.
5. The measured temperature is shown on the 16x2 LCD.
6. When the displayed temperature reaches 50.0 °C, the buzzer and red LED are enabled.
7. They stay on until the temperature drops below 48.0 °C (2 °C hysteresis), so the alarm does not chatter near the threshold.
8. A raw reading of 5 or less, or 4090 or more, is treated as a sensor fault: the LCD shows `SENSOR FAULT` and the alarm turns on. On the first good reading after a fault, the alarm follows the current temperature again.

## Calculation

`Vout = ADC_count × 3.3 / 4095`

`Temperature_C = Vout × 100`

The ADC reference and sensor supply should be measured/verified in the physical implementation for best accuracy.
