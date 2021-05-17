/*
 * lcd_util.c
 *
 *  Created on: Feb 22, 2018
 *      Author: gerd
 */

#include "lcd_util.h"
#include <string.h>
#include <stdio.h>

void lcd_print_two_lines(const char* line1, const char* line2)
{
	lcd_clrscr();
	lcd_puts(line1);
	lcd_gotoxy(0, 1);
	lcd_puts(line2);
}

void lcd_print_uint8_t_decimal(const char* name, uint8_t value)
{
	static char buf[3+1];
    snprintf(buf, 3+1, "%u", value);
	lcd_print_two_lines(name, buf);
}

void lcd_print_uint8_t_hex(const char* name, uint8_t value)
{
	static char buf[4+1];
    snprintf(buf, 4+1, "%X", value);
	lcd_print_two_lines(name, buf);
}

void lcd_print_uint16_t_decimal(const char* name, uint16_t value)
{
	static char buf[5+1];
    snprintf(buf, 5+1, "%u", value);
	lcd_print_two_lines(name, buf);
}

void lcd_print_two_bits(const char* name1, uint8_t byte1, uint8_t bit1, const char* name2, uint8_t byte2, uint8_t bit2)
{
    char line1[16 + 1];
    char line2[16 + 1];

    uint8_t bit1IsSet = (byte1 & (1 << bit1)) != 0;
    uint8_t bit2IsSet = (byte2 & (1 << bit2)) != 0;
    snprintf(line1, 16+1, "%s is %s", name1, (bit1IsSet) ? "set" : "unset");
    snprintf(line2, 16+1, "%s is %s", name2, (bit2IsSet) ? "set" : "unset");
	lcd_print_two_lines(line1, line2);
}

void lcd_print_two_uint8s(const char* name1, uint8_t value1, const char* name2, uint8_t value2)
{
	char line1[8 + 1];
	char line2[8 + 1];
	snprintf(line1, sizeof(line1), "%s: %u", name1, value1);
	snprintf(line2, sizeof(line2), "%s: %u", name2, value2);
	lcd_print_two_lines(line1, line2);
}

void lcd_print_two_uint16s(const char* name1, uint16_t value1, const char* name2, uint16_t value2)
{
	char line1[16 + 1];
	char line2[16 + 1];
	snprintf(line1, sizeof(line1), "%s: %u", name1, value1);
	snprintf(line2, sizeof(line2), "%s: %u", name2, value2);
	lcd_print_two_lines(line1, line2);
}