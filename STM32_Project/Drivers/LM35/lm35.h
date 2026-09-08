#ifndef LM35_H
#define LM35_H

#include "main.h"

#define LM35_ADC_MAX_VALUE       4095.0f
#define LM35_ADC_REFERENCE_VOLT  3.3f
#define LM35_ALARM_THRESHOLD_C   50.0f

void LM35_Init(ADC_HandleTypeDef *hadc);
float LM35_ReadTemperatureC(void);
uint32_t LM35_ReadRaw(void);

#endif
