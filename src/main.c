

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

    PORTB = 0;
    PORTC = 0;

    // LED on Port C Pin 5
    DDRC = (1 << PIN5);

    for (;;)
    {
        // Turn LED on
        PORTC &= ~(1 << PIN5);        
        lcd_print_two_lines("Hello World", "foo");
        _delay_ms(500);

        // Turn LED off
        PORTC |= (1 << PIN5);
        lcd_clrscr();
        _delay_ms(500);
    }
}



