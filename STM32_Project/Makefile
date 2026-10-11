# Build the STM32F103C8T6 firmware with arm-none-eabi-gcc.
#
#   git clone --depth 1 https://github.com/STMicroelectronics/stm32f1xx_hal_driver ../third_party/hal
#   git clone --depth 1 https://github.com/STMicroelectronics/cmsis_device_f1      ../third_party/device
#   git clone --depth 1 https://github.com/ARM-software/CMSIS_5                    ../third_party/CMSIS_5
#   make            # -> build/stm32_temperature.elf, .hex and .bin
#
# Flash build/stm32_temperature.bin at 0x08000000 with ST-Link or STM32CubeProgrammer.

HAL_DIR    ?= ../third_party/hal
DEVICE_DIR ?= ../third_party/device
CMSIS_DIR  ?= ../third_party/CMSIS_5
BUILD      ?= build
TARGET      = stm32_temperature

CC      = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE    = arm-none-eabi-size

CPU     = -mcpu=cortex-m3 -mthumb
DEFS    = -DUSE_HAL_DRIVER -DSTM32F103xB
INCS    = -ICore/Inc -IDrivers/LM35 -IDrivers/LCD -IDrivers/Buzzer -IDrivers/LED \
          -I$(HAL_DIR)/Inc -I$(DEVICE_DIR)/Include -I$(CMSIS_DIR)/CMSIS/Core/Include
CFLAGS  = $(CPU) $(DEFS) $(INCS) -std=gnu11 -Os -g -Wall -Wextra -ffunction-sections -fdata-sections
LDFLAGS = $(CPU) -T$(DEVICE_DIR)/Source/Templates/gcc/linker/STM32F103XB_FLASH.ld \
          -specs=nano.specs -specs=nosys.specs -Wl,--gc-sections -Wl,-Map=$(BUILD)/$(TARGET).map

APP_SRC = Core/Src/main.c Core/Src/stm32f1xx_it.c \
          Drivers/LM35/lm35.c Drivers/LCD/lcd.c Drivers/Buzzer/buzzer.c Drivers/LED/led.c
HAL_SRC = $(addprefix $(HAL_DIR)/Src/stm32f1xx_, hal.c hal_adc.c hal_adc_ex.c hal_cortex.c hal_dma.c \
          hal_flash.c hal_flash_ex.c hal_gpio.c hal_pwr.c hal_rcc.c hal_rcc_ex.c)
SYS_SRC = $(DEVICE_DIR)/Source/Templates/system_stm32f1xx.c
ASM_SRC = $(DEVICE_DIR)/Source/Templates/gcc/startup_stm32f103xb.s

OBJS = $(addprefix $(BUILD)/,$(notdir $(APP_SRC:.c=.o) $(HAL_SRC:.c=.o) $(SYS_SRC:.c=.o) $(ASM_SRC:.s=.o)))
vpath %.c $(sort $(dir $(APP_SRC) $(HAL_SRC) $(SYS_SRC)))
vpath %.s $(dir $(ASM_SRC))

all: $(BUILD)/$(TARGET).elf $(BUILD)/$(TARGET).hex $(BUILD)/$(TARGET).bin
	$(SIZE) $(BUILD)/$(TARGET).elf

$(BUILD)/%.o: %.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: %.s | $(BUILD)
	$(CC) $(CPU) -c $< -o $@

$(BUILD)/$(TARGET).elf: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

$(BUILD)/%.hex: $(BUILD)/%.elf
	$(OBJCOPY) -O ihex $< $@

$(BUILD)/%.bin: $(BUILD)/%.elf
	$(OBJCOPY) -O binary $< $@

$(BUILD):
	mkdir -p $@

clean:
	rm -rf $(BUILD)

.PHONY: all clean
