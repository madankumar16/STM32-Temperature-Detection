#include "temp_logic.h"

#include <stdio.h>

float TempLogic_RawToCelsius(uint32_t raw)
{
    float voltage = ((float)raw * LM35_ADC_REFERENCE_VOLT) / LM35_ADC_MAX_VALUE;
    return voltage * 100.0f;
}

uint32_t TempLogic_CelsiusToTenths(float celsius)
{
    if (!(celsius > 0.0f))          /* also catches NaN */
        return 0U;
    if (celsius > 999.9f)
        return 9999U;
    return (uint32_t)(celsius * 10.0f + 0.5f);
}

uint32_t TempLogic_AverageRaw(const uint32_t *samples, uint32_t count)
{
    uint64_t total = 0U;

    if (samples == NULL || count == 0U)
        return LM35_RAW_INVALID;

    for (uint32_t i = 0U; i < count; ++i)
    {
        if (samples[i] == LM35_RAW_INVALID)
            return LM35_RAW_INVALID;
        total += samples[i];
    }

    return (uint32_t)((total + (count / 2U)) / count);
}

int TempLogic_IsSensorFault(uint32_t raw)
{
    return raw <= LM35_SENSOR_LOW_RAW_THRESHOLD ||
           raw >= LM35_SENSOR_HIGH_RAW_THRESHOLD;
}

void TempLogic_Init(TempAlarmState *state)
{
    state->alarm_on = 0;
    state->in_fault = 0;
}

TempStatus TempLogic_Update(TempAlarmState *state, uint32_t raw)
{
    TempStatus status = {0, 0, 0U};

    if (TempLogic_IsSensorFault(raw))
    {
        state->in_fault = 1;
        status.fault = 1;
        status.alarm = 1;           /* fail safe */
        return status;
    }

    status.tenths = TempLogic_CelsiusToTenths(TempLogic_RawToCelsius(raw));

    if (state->in_fault)
    {
        /* Recovering from a fault: forget the forced alarm and start the
         * hysteresis again from the current temperature. */
        state->in_fault = 0;
        state->alarm_on = (status.tenths >= LM35_ALARM_ON_TENTHS);
    }
    else if (status.tenths >= LM35_ALARM_ON_TENTHS)
    {
        state->alarm_on = 1;
    }
    else if (status.tenths < LM35_ALARM_OFF_TENTHS)
    {
        state->alarm_on = 0;
    }

    status.alarm = state->alarm_on;
    return status;
}

void TempLogic_FormatLine(char *buffer, size_t buffer_size, uint32_t tenths)
{
    int len;

    if (buffer == NULL || buffer_size == 0U)
        return;

    /* Integer formatting so newlib-nano does not need printf float support. */
    if (tenths > 9999U)
        tenths = 9999U;
    len = snprintf(buffer, buffer_size, "%lu.%lu C",
                   (unsigned long)(tenths / 10U),
                   (unsigned long)(tenths % 10U));
    if (len < 0)
        len = 0;

    /* Pad with spaces to the full LCD width so old characters are overwritten. */
    for (size_t i = (size_t)len; i + 1U < buffer_size && i < TEMP_LCD_COLUMNS; ++i)
        buffer[i] = ' ';
    if (buffer_size > TEMP_LCD_COLUMNS)
        buffer[TEMP_LCD_COLUMNS] = '\0';
    else
        buffer[buffer_size - 1U] = '\0';
}
