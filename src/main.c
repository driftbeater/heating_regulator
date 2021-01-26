

#ifndef F_CPU
#error "F_CPU not defined "
#endif

#include <avr/io.h>
#include <delay.h>
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>


#include "LCD162C/lcd_util.h"
#include "c_util/string_util.h"


void init_adc()
{

    // Enable ADC
    // Set ADEN bit in ADCSRA (AD Control and Status Register A)
    ADCSRA |= _BV(ADEN);

    // Configure prescaler so a frequency of 200kHz results
    // 8'000'000 / 200'000 = 40
    // ADPS1 und ADPS2 -> 64. 8'000'000 / 64 = 125kHz
    ADCSRA |=  _BV(ADPS2);
    ADCSRA |=  _BV(ADPS1);
    ADCSRA &= ~_BV(ADPS0);

    // Set single conversion mode
    ADCSRA &= ~_BV(ADFR);

    // Disable Conversion Complete Interrupt
    ADCSRA &= ~_BV(ADIE);

    // Voltage Reference selection
    // Internal Ref voltage with capacitor at AREF pin
    ADMUX |= _BV(REFS1);
    ADMUX |= _BV(REFS0);

    // Right adjust result
    ADMUX &= ~_BV(ADLAR);

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

    lcd_print_two_bits("ADSC", ADCSRA, ADSC, "ADSC", ADCSRA, ADSC );
    // todo wait for bit ??

    loop_until_bit_is_clear(ADCSRA, ADSC);

   lcd_print_uint16_t_decimal("ADCW", ADCW);
    _delay_ms(2000);

    return ADCW;

}

void init_pwm()
{
    // OC1A is output on Port B Pin 1
    DDRB |= _BV(PB1);
   
    // Non-inverting. COM1A1:1 COM1A0:0
    TCCR1A |= (1 << COM1A1);

    // PWM Mode. 8 bit resolution
    TCCR1A &= ~_BV(WGM12);
    TCCR1A &= ~_BV(WGM11);
    TCCR1A |=  _BV(WGM10);

    // Prescaler. Clock 8'000'000 / 1024 = 7800Hz. Frequency = 8'000'000 / 1'024 / 510 (in 8 bit mode) = 15.3Hz

    TCCR1B |= _BV(CS12);
    TCCR1B &= _BV(CS11);
    TCCR1B |= _BV(CS10);
 
}

int main()
{
    // init AVR
    _delay_ms(0);
    cli();

    // init LCD Display
    lcd_init(LCD_DISP_ON);

    // LED on Port C Pin 5
    DDRC = (1 << PIN5);
    uint8_t led = 0U;

    // A/D Converter on Port C Pin 4
    init_adc();
    lcd_print_two_lines("ADC init", "done");
    _delay_ms(2000);

    // PWM Signal on output OC1
    init_pwm();   

    for (;;)
    {
        lcd_clrscr();

        // uint8_t v = 0xFF;
        // lcd_print_two_lines("0xFF", uint8_t_to_bin(v));
        // _delay_ms(2000);
        // v = 0x1F;
        // lcd_print_two_lines("0x1F", uint8_t_to_bin(v));
        // _delay_ms(2000);
        // v = 0x73;
        // lcd_print_two_lines("0x73", uint8_t_to_bin(v));
        // _delay_ms(2000);
        
        // Toggle LED
        //PORTC  ^= _BV(PIN5);
        
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
        _delay_ms(1000);
        for (uint16_t x = 10; x >= 0; --x)
        {
           char buf[8];
           snprintf(buf, 8, "to %d", x);
           lcd_print_two_lines("Setting OCR1A", buf);
           OCR1A = x;
           _delay_ms(4000);
        }
    }
}
