

#ifndef F_CPU
#error "F_CPU not defined "  
#endif

#include <avr/io.h>
#include <delay.h>
#include "LCD162C/lcd_util.h"

int main()
{
    // init AVR
    _delay_ms(0);

    lcd_init(LCD_DISP_ON);

    for (;;)
    {
        _delay_ms(500);
        lcd_print_two_lines("Hello World", "foo");
        _delay_ms(500);
        lcd_clrscr();
    }
}



