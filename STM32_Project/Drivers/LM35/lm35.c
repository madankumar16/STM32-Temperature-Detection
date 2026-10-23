#include "lm35.h"

static ADC_HandleTypeDef *lm35_adc = NULL;

void LM35_Init(ADC_HandleTypeDef *hadc)
{
    lm35_adc = hadc;
}

static uint32_t LM35_ReadSingle(void)
{
    uint32_t value;

    if (HAL_ADC_Start(lm35_adc) != HAL_OK)
        return LM35_RAW_INVALID;

    if (HAL_ADC_PollForConversion(lm35_adc, 100U) == HAL_OK)
        value = HAL_ADC_GetValue(lm35_adc);
    else
        value = LM35_RAW_INVALID;

    if (HAL_ADC_Stop(lm35_adc) != HAL_OK)
        return LM35_RAW_INVALID;

    return value;
}

/* Average LM35_SAMPLE_COUNT conversions to reduce noise.
 * Returns LM35_RAW_INVALID if any conversion fails, which the caller
 * treats as a sensor fault. */
uint32_t LM35_ReadRaw(void)
{
    uint32_t samples[LM35_SAMPLE_COUNT];

    if (lm35_adc == NULL)
        return LM35_RAW_INVALID;

    for (uint32_t i = 0U; i < LM35_SAMPLE_COUNT; ++i)
    {
        samples[i] = LM35_ReadSingle();
        if (samples[i] == LM35_RAW_INVALID)
            return LM35_RAW_INVALID;
    }

    return TempLogic_AverageRaw(samples, LM35_SAMPLE_COUNT);
}

float LM35_ConvertRawToTemperatureC(uint32_t raw)
{
    return TempLogic_RawToCelsius(raw);
}

float LM35_ReadTemperatureC(void)
{
    return LM35_ConvertRawToTemperatureC(LM35_ReadRaw());
}
