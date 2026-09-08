#ifndef ADC_H
#define ADC_H

#include "main.h"

extern ADC_HandleTypeDef hadc1;
void ADC1_UserInit(void);
uint32_t ADC1_ReadChannel0(void);

#endif
