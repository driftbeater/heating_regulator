/*
 * lcd_util.h
 *
 *  Created on: Feb 22, 2018
 *      Author: gerd
 */

#ifndef LCD_UTIL_H_
#define LCD_UTIL_H_

#include "lcd.h"

void lcd_print_two_lines(const char* line1, const char* line2);

void lcd_print_uint8_t_decimal(const char* name, uint8_t value);
void lcd_print_uint8_t_hex(const char* name, uint8_t value);
void lcd_print_uint16_t_decimal(const char* name, uint16_t value);

void lcd_print_two_bits(const char* name1, uint8_t byte1, uint8_t bit1, const char* name2, uint8_t byte2, uint8_t bit2);
#endif /* LCD_UTIL_H_ */
