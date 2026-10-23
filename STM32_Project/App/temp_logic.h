#ifndef TEMP_LOGIC_H
#define TEMP_LOGIC_H

/*
 * Pure temperature logic: raw ADC -> degrees C, averaging, sensor-fault
 * check, alarm hysteresis and LCD line formatting.
 *
 * This file has no HAL dependency so it builds with the host gcc and is
 * covered by the unit tests in tests/ (run: make -C STM32_Project test).
 */

#include <stddef.h>
#include <stdint.h>

#define LM35_ADC_MAX_VALUE       4095.0f
#define LM35_ADC_REFERENCE_VOLT  3.3f
#define LM35_SAMPLE_COUNT        16U     /* readings averaged per update */

/* Alarm thresholds, in tenths of a degree so the alarm decision uses
 * exactly the value shown on the LCD. */
#define LM35_ALARM_ON_TENTHS     500U    /* alarm turns on at 50.0 C       */
#define LM35_ALARM_OFF_TENTHS    480U    /* alarm turns off below 48.0 C   */

/* Raw ADC values at or beyond these mean an open or shorted sensor. */
#define LM35_SENSOR_LOW_RAW_THRESHOLD   5U
#define LM35_SENSOR_HIGH_RAW_THRESHOLD  4090U

/* Returned for a failed ADC conversion; always treated as a sensor fault. */
#define LM35_RAW_INVALID         UINT32_MAX

/* Width of one LCD line, without the terminating NUL. */
#define TEMP_LCD_COLUMNS         16U

typedef struct
{
    int alarm_on;   /* 1 while the LED and buzzer should be on            */
    int in_fault;   /* 1 while the last reading was a sensor fault         */
} TempAlarmState;

typedef struct
{
    int fault;          /* 1 = sensor fault, temperature not valid         */
    int alarm;          /* 1 = drive the LED and buzzer                    */
    uint32_t tenths;    /* temperature in 0.1 C (only valid if !fault)     */
} TempStatus;

/* Convert a 12-bit ADC count to degrees C (LM35: 10 mV per degree). */
float TempLogic_RawToCelsius(uint32_t raw);

/* Round degrees C to tenths, clamped to 0.0 .. 999.9. */
uint32_t TempLogic_CelsiusToTenths(float celsius);

/* Rounded mean of count samples. Returns LM35_RAW_INVALID if count is 0
 * or any sample is LM35_RAW_INVALID. */
uint32_t TempLogic_AverageRaw(const uint32_t *samples, uint32_t count);

/* 1 if the raw value means the sensor is disconnected or shorted. */
int TempLogic_IsSensorFault(uint32_t raw);

/* Start with the alarm off and no fault. */
void TempLogic_Init(TempAlarmState *state);

/*
 * Process one averaged raw reading and decide the alarm.
 *
 *  - Sensor fault: alarm on (fail safe).
 *  - Normal: alarm turns on at >= 50.0 C and stays on until the
 *    temperature drops below 48.0 C (2 C hysteresis, no buzzer chatter).
 *  - First good reading after a fault: the hysteresis state is reset, so
 *    the alarm simply reflects the current temperature (on only if >= 50.0 C).
 */
TempStatus TempLogic_Update(TempAlarmState *state, uint32_t raw);

/* Write "NN.N C" padded with spaces to exactly TEMP_LCD_COLUMNS characters
 * plus a NUL. buffer_size must be at least TEMP_LCD_COLUMNS + 1. */
void TempLogic_FormatLine(char *buffer, size_t buffer_size, uint32_t tenths);

#endif
