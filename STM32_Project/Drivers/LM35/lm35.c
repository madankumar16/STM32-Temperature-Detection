#include "lm35.h"

static ADC_HandleTypeDef *lm35_adc = NULL;

void LM35_Init(ADC_HandleTypeDef *hadc)
{
    lm35_adc = hadc;
}

uint32_t LM35_ReadRaw(void)
{
    uint32_t value = 0;

    if (lm35_adc == NULL)
        return 0;

    HAL_ADC_Start(lm35_adc);
    if (HAL_ADC_PollForConversion(lm35_adc, 100) == HAL_OK)
        value = HAL_ADC_GetValue(lm35_adc);
    HAL_ADC_Stop(lm35_adc);

    return value;
}

float LM35_ReadTemperatureC(void)
{
    uint32_t raw = LM35_ReadRaw();
    float voltage = ((float)raw * LM35_ADC_REFERENCE_VOLT) / LM35_ADC_MAX_VALUE;
    return voltage * 100.0f;
}
