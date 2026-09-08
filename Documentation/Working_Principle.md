# Working Principle

1. The LM35 produces an analog voltage proportional to temperature, approximately 10 mV per degree Celsius.
2. STM32 ADC1 samples the sensor voltage on PA0 using a 12-bit conversion.
3. Firmware converts the ADC count to voltage using a 3.3 V reference assumption.
4. The voltage is multiplied by 100 to obtain temperature in Celsius.
5. The measured temperature is shown on the 16x2 LCD.
6. When the temperature reaches or exceeds 50 °C, the buzzer and red LED are enabled.
7. Below the threshold, both alarm outputs are disabled.

## Calculation

`Vout = ADC_count × 3.3 / 4095`

`Temperature_C = Vout × 100`

The ADC reference and sensor supply should be measured/verified in the physical implementation for best accuracy.
