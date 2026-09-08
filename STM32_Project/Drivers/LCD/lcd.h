#ifndef LCD_H
#define LCD_H

#include "main.h"

void LCD_Init(void);
void LCD_SendCommand(uint8_t command);
void LCD_SendData(uint8_t data);
void LCD_SetCursor(uint8_t row, uint8_t column);
void LCD_Print(const char *text);
void LCD_Clear(void);

#endif
