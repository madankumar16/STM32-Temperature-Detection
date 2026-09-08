#include "lcd.h"

#define LCD_RS_PORT GPIOB
#define LCD_RS_PIN  GPIO_PIN_12
#define LCD_EN_PORT GPIOB
#define LCD_EN_PIN  GPIO_PIN_13
#define LCD_D4_PORT GPIOB
#define LCD_D4_PIN  GPIO_PIN_14
#define LCD_D5_PORT GPIOB
#define LCD_D5_PIN  GPIO_PIN_15
#define LCD_D6_PORT GPIOA
#define LCD_D6_PIN  GPIO_PIN_8
#define LCD_D7_PORT GPIOA
#define LCD_D7_PIN  GPIO_PIN_9

static void LCD_Write4(uint8_t nibble)
{
    HAL_GPIO_WritePin(LCD_D4_PORT, LCD_D4_PIN, (nibble & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D5_PORT, LCD_D5_PIN, (nibble & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D6_PORT, LCD_D6_PIN, (nibble & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D7_PORT, LCD_D7_PIN, (nibble & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_RESET);
    HAL_Delay(1);
}

void LCD_SendCommand(uint8_t command)
{
    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_RESET);
    LCD_Write4(command >> 4);
    LCD_Write4(command & 0x0F);
    HAL_Delay(2);
}

void LCD_SendData(uint8_t data)
{
    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_SET);
    LCD_Write4(data >> 4);
    LCD_Write4(data & 0x0F);
}

void LCD_Init(void)
{
    HAL_Delay(40);
    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_RESET);
    LCD_Write4(0x03); HAL_Delay(5);
    LCD_Write4(0x03); HAL_Delay(1);
    LCD_Write4(0x03); HAL_Delay(1);
    LCD_Write4(0x02);

    LCD_SendCommand(0x28); // 4-bit, 2-line, 5x8 font
    LCD_SendCommand(0x0C); // display on, cursor off
    LCD_SendCommand(0x06); // entry mode
    LCD_Clear();
}

void LCD_Clear(void)
{
    LCD_SendCommand(0x01);
    HAL_Delay(2);
}

void LCD_SetCursor(uint8_t row, uint8_t column)
{
    uint8_t address = (row == 0) ? 0x80 : 0xC0;
    LCD_SendCommand(address + column);
}

void LCD_Print(const char *text)
{
    while (*text)
        LCD_SendData((uint8_t)*text++);
}
