

#ifndef F_CPU
#error "F_CPU not defined "
#endif

#include <avr/io.h>
#include <delay.h>
#include "LCD162C/lcd_util.h"
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

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
    // Internal Ref voltage with capacitor at AREF pin
    ADMUX |= (1 << REFS1);
    ADMUX |= (1 << REFS0);

    // Right adjust result
    ADMUX &= ~(1 << ADLAR);

    set_sleep_mode(SLEEP_MODE_ADC);

}

ISR(ADC_vect)
{
    // disable interrupts
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

    sleep_mode();


    uint16_t x = ADCW;
    lcd_print_uint16_t_decimal("ADCW", x);
    _delay_ms(100);

    return x;

}

int main()
{
    // init AVR
    _delay_ms(0);
    sei();

    // init LCD Display
    lcd_init(LCD_DISP_ON);

    // LED on Port C Pin 5
    DDRC = (1 << PIN5);

    // A/D Converter on Port C Pin 4
    init_adc();
    lcd_print_two_lines("ADC init", "done");
    _delay_ms(2000);
    uint8_t led = 0U;

    for (;;)
    {
        lcd_clrscr();
        
        // Toggle LED
        //PORTC  ^= (1 << PIN5);
        led ^= 1U;
        if (led != 0)
        {
            PORTC |= (led << PIN5);
        }   
        else
        {
            PORTC &= ~(1 << PIN5);
        }
        

        //_delay_ms(100);

        // Do A/D conversion
        uint16_t value = convert_atod();

        lcd_print_uint16_t_decimal("ADCW", value);   
        //_delay_ms(1);

    }
}
