/*
 * lcd.h
 *
 *  Created on: Nov 14, 2025
 *      Author: pulkitp
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_


#include "main.h"



#define SCREEN_CHANGE_TIME			 (2U) // 5*1000ms = 5 second
#define SETTING_HOLD 				 (20U)// 10*100ms = 1 second
#define SCREEN_BACK_CHANGE			 (5U)



void LCD_Init(void);
void lcd_clear(void);
void LCD_Command(uint8_t cmd);
void LCD_Data(uint8_t data);
void LCD_SendString(char *str);
void LCD_Write4Bits(uint8_t data);
void LCD_PulseEnable(void);
void lcd_print(unsigned char pos_x, unsigned char pos_y, char *message);







#endif /* INC_LCD_H_ */
