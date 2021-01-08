

#ifndef F_CPU
#error "F_CPU not defined "
#endif

#include <avr/io.h>
#include <delay.h>
#include "LCD162C/lcd_util.h"

void init_adc()
{
    // Enable ADC
    // Set ADEN bit in ADCSRA (AD Control and Status Register A)
    ADCSRA |= (1 << ADEN);

    // Configure prescaler so a frequency of 200kHz results
    // 8'000'000 / 200'000 = 40
    // ADPS1 und ADPS2 -> 64. 8'000'000 / 64 = 125kHz
    ADCSRA |=  (1 << ADPS2);
    ADCSRA |=  (1 << ADPS1);
    ADCSRA &= ~(1 << ADPS0);

    // Set single conversion mode
    ADCSRA &= ~(1 << ADFR);

    // Enable Conversion Complete Interrupt
    ADCSRA |= (1 << ADIE);

    // todo set I-bit in SREG

    // Voltage Reference selection
    // AVCC with external capacitor at AREF: 10
    ADMUX &= ~(1 << REFS1);
    ADMUX |=  (1 << REFS0);

    // Right adjust result
    ADMUX &= ~(1 << ADLAR);
}

uint16_t convert_atod()
{
    // Select channel
    // ADC4 0100
    ADMUX &= ~(1 << MUX3);
    ADMUX |=  (1 << MUX2);
    ADMUX &= ~(1 << MUX1);
    ADMUX &= ~(1 << MUX0);

    // Start Conversion
    ADCSRA |= (1 << ADSC);

    _delay_ms(100);
    // Make sure ADIF bit is unset

    uint8_t tmp = ADCSRA;
    if ( (tmp & (1 << ADIF)) != 0)
    {
       lcd_print_two_lines("Warning", "ADIF is set");
       // return 0;
    }
    if ( (tmp & (1 << ADIF)) == 0)
    {
       lcd_print_two_lines("Warning", "ADIF is set");
       // return 0;
    }

    uint16_t x = ADCW;
    lcd_print_uint16_t_decimal("ADCW", x);

    return x;

}

int main()
{
    // init AVR
    _delay_ms(0);

    // init LCD Display
    lcd_init(LCD_DISP_ON);

    // LED on Port C Pin 5
    DDRC = (1 << PIN5);

    // A/D Converter on Port C Pin 4
    init_adc();
    lcd_print_two_lines("ADC init", "done");
    _delay_ms(2000);

    for (;;)
    {
        lcd_clrscr();
        
        // Turn LED on
        PORTC &= ~(1 << PIN5);
        // lcd_print_two_lines("LED turned on", "");
        // _delay_ms(2000);

        // Do A/D conversion
        uint16_t value = convert_atod();

        // Turn LED off
        PORTC |= (1 << PIN5);
        // lcd_print_two_lines("LED turned off", "");
        // _delay_ms(2000);

        lcd_print_uint16_t_decimal("ADCW", value);   
        _delay_ms(1000);

    }
}
