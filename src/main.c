

#ifndef F_CPU
#error "F_CPU not defined "
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>


#include "lcd_util.h"
#include "string_util.h"
#include "NTC_B7703M103G40_10k.h"

const uint16_t POWERRESISTOR = 33;

uint16_t getPwmRate(uint16_t voltageMilli, uint16_t powerMilli)
{
  uint16_t pmaxMilli = voltageMilli ^ 2 / POWERRESISTOR;
  uint16_t pwmProMill = (uint16_t)1000 * powerMilli / pmaxMilli;
  return pwmProMill;
}

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

void select_channel_adc3()
{
    // Select channel ADC3: 0011
    ADMUX &= ~_BV(MUX3);
    ADMUX &= ~_BV(MUX2);
    ADMUX |=  _BV(MUX1);
    ADMUX |=  _BV(MUX0);
}

void select_channel_adc4()
{
    // Select channel ADC4: 0100
    ADMUX &= ~_BV(MUX3);
    ADMUX |=  _BV(MUX2);
    ADMUX &= ~_BV(MUX1);
    ADMUX &= ~_BV(MUX0);
}

void select_channel_adc5()
{
    // Select channel ADC5: 0101
    ADMUX &= ~_BV(MUX3);
    ADMUX |=  _BV(MUX2);
    ADMUX &= ~_BV(MUX1);
    ADMUX |=  _BV(MUX0);
}

uint16_t convert_atod()
{

    // Start Conversion
    ADCSRA |= _BV(ADSC);

    lcd_print_two_bits("ADSC", ADCSRA, ADSC, "ADSC", ADCSRA, ADSC );
    // todo wait for bit ??

    loop_until_bit_is_clear(ADCSRA, ADSC);

    // lcd_print_uint16_t_decimal("ADCW", ADCW);
    // _delay_ms(2000);

    return ADCW;

}
void init_pwm_counter2()
{
    // OC2 is output on Port B Pin 3
    DDRB |= _BV(PB3);

    // Internal clock. Prescaler 1/1024. Frequency 8'000'000 / 1024 / 510 = 15Hz
    // TCCR2 |= _BV(CS22) | _BV(CS21) | _BV(CS20);
    
    // Internal clock. Prescaler 1/256. Frequency 8'000'000 / 256 / 510 = 61 = Hz
    // TCCR2 |= _BV(CS22) | _BV(CS21);
    
    // Internal clock. Prescaler 1/128. Frequency 8'000'000 / 128 / 510 = 122 = Hz
    // TCCR2 |= _BV(CS22) | _BV(CS20);
    
    // Internal clock. Prescaler 1/128. Frequency 8'000'000 / 64 / 510 = 245 = Hz
    TCCR2 |= _BV(CS22);

    // Phase correct PWM mode
    // TCCR2 |= _BV(WGM20);
    
    // Internal clock. Prescaler 1/128. Frequency 8'000'000 / 64 / 255 = 490 = Hz
    // Fast PWM mode
    TCCR2 |= _BV(WGM21);
    TCCR2 |= _BV(WGM20);

    // Non inverting
    TCCR2 |= _BV(COM21) | _BV(COM20);

    // Inverting
    // TCCR2 |= _BV(COM21);

}

void init_pwm_counter1()
{
    // OC1A is output on Port B Pin 1
    DDRB |= _BV(PB1);

    // External clock source. No Prescaler possible. 
    TCCR1B |= _BV(CS12) | _BV(CS11) | _BV(CS10);

    // Phase correct PWM Mode. 8 bit resolution. Frequency = input frequence / 510 in phase correcct pwm mode. => 490 / 510 = 0.5Hz
    // TCCR1A |=  _BV(WGM10);

    // Fast PWM Mode. 8 bit resolution. Frequency = input frequence / 510 in phase correcct pwm mode. => 490 / 255 = 2Hz
    TCCR1A |= _BV(WGM12) | _BV(WGM10);

    // Inverting. COM1A1:1 COM1A0:1
    TCCR1A |= _BV(COM1A1) | _BV(COM1A0);
 
}

int main()
{
    // init AVR
    _delay_ms(0);
    cli();

    // init LCD Display
    lcd_init(LCD_DISP_ON);

    // A/D Converter on Port C Pin 3 and 4 
    init_adc();
    lcd_print_two_lines("ADC init", "done");
    _delay_ms(2000);

    // PWM Signal on output OC1
    init_pwm_counter1();   
    init_pwm_counter2();   

    for (;;)
    {

        lcd_clrscr();

        // Do A/D conversion on ADC3
        select_channel_adc3();
        uint16_t value_adc3 = convert_atod();
        // 42V -> 2.21V
        // 2.21V = 2.56 * x / 1024 -> x = 884
        uint16_t v0 = value_adc3 * (uint16_t)42 / (uint16_t)884;
        lcd_print_two_uint8s("ADC3", value_adc3, "v0", v0);
        _delay_ms(2000);

        // Do A/D conversion on ADC4
        select_channel_adc4();
        uint16_t value_adc4 = convert_atod();
        uint16_t tempCooler = getRNP50UTempFromAdcValue(value_adc4);
        uint16_t PDerating = getRNP50UDeratedPower(tempCooler);

        lcd_print_two_uint8s("ADC4", value_adc4, "TempCooler", tempCooler);
        _delay_ms(2000);

        lcd_print_uint8_t_decimal("max power", PDerating);
        _delay_ms(2000);

        // Do A/D conversion on ADC5
        select_channel_adc5();
        uint16_t value_adc5 = convert_atod();
        uint16_t tempAir = getRNP50UTempFromAdcValue(value_adc5);

        lcd_print_two_uint8s("ADC5", value_adc5, "TempAir", tempAir);
        _delay_ms(2000);

        // Calculation of PWM rate
        uint16_t Pmax = v0 * v0 / POWERRESISTOR;
        lcd_print_two_uint8s("Pmax: %u", Pmax, "PDerating", PDerating);
        _delay_ms(2000);

        // for (uint16_t x = 0; x <= 250; x = x + 1)
        // {
        //   char buf[16+1];
        //   snprintf(buf, 16+1, "to %d", x);
        //   lcd_print_two_lines("Setting OCR1A", buf);
        //   // First counter's PWM is always 50%
        uint16_t x = 10;
        OCR2 = 128;
        OCR1A = x;
        //   _delay_ms(3000);
        // }
    }
}
