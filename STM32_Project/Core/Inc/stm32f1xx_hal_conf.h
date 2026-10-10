/*
 * Minimal STM32F1 HAL configuration for this project.
 * Target: STM32F103C8T6 (Blue Pill), HSI 8 MHz.
 */
#ifndef STM32F1xx_HAL_CONF_H
#define STM32F1xx_HAL_CONF_H

#define HAL_MODULE_ENABLED
#define HAL_ADC_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED

#define HSE_VALUE                    8000000U
#define HSE_STARTUP_TIMEOUT          100U
#define HSI_VALUE                    8000000U
#define LSI_VALUE                    40000U
#define LSE_VALUE                    32768U
#define LSE_STARTUP_TIMEOUT          5000U
#define EXTERNAL_CLOCK_VALUE         12288000U
#define VDD_VALUE                    3300U
#define TICK_INT_PRIORITY            15U
#define USE_RTOS                     0U
#define PREFETCH_ENABLE              1U
#define USE_SPI_CRC                  0U

#ifndef assert_param
#define assert_param(expr) ((void)0U)
#endif

#endif /* STM32F1xx_HAL_CONF_H */
