#ifndef LM35_H
#define LM35_H

#include "main.h"
#include "temp_logic.h"   /* thresholds, conversion and alarm logic (no HAL) */

void LM35_Init(ADC_HandleTypeDef *hadc);
float LM35_ReadTemperatureC(void);
float LM35_ConvertRawToTemperatureC(uint32_t raw);
uint32_t LM35_ReadRaw(void);

#endif
