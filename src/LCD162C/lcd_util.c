/*
 * lcd_util.c
 *
 *  Created on: Feb 22, 2018
 *      Author: gerd
 */

#include "lcd_util.h"
#include <string.h>

void lcd_print_two_lines(const char* line1, const char* line2)
{
	lcd_clrscr();
	lcd_puts(line1);
	lcd_gotoxy(0, 1);
	lcd_puts(line2);
}
