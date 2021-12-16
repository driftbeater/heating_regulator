

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
    ADCSRA |= _BV(ADPS2);
    ADCSRA |= _BV(ADPS1);
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

void select_channel_adc2()
{
    // Select channel ADC2: 0010
    ADMUX &= ~_BV(MUX3);
    ADMUX &= ~_BV(MUX2);
    ADMUX |=  _BV(MUX1);
    ADMUX &= ~_BV(MUX0);
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
    ADMUX |= _BV(MUX2);
    ADMUX &= ~_BV(MUX1);
    ADMUX |= _BV(MUX0);
}

uint16_t convert_atod()
{

    // Start Conversion
    ADCSRA |= _BV(ADSC);

    // lcd_print_two_bits("ADSC", ADCSRA, ADSC, "ADSC", ADCSRA, ADSC );
    // todo wait for bit ??

    loop_until_bit_is_clear(ADCSRA, ADSC);

    // lcd_print_uint16_t_decimal("ADCW", ADCW);
    // _delay_ms(2000);

    return ADCW;
}
void init_pwm_counter2()
{
    // 8 Bit Timer/Counter 2

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

    // First counter's PWM is always 50%. It is only used as a clock source for second counter
    OCR2 = 128;
}

void init_pwm_counter1()
{
    // 16Bit Timer/Counter 1


    // External clock source on T1 i.e. PD5. No Prescaler possible.
    TCCR1B |= _BV(CS12) | _BV(CS11) | _BV(CS10);

    // Phase correct PWM Mode. 8 bit resolution. Frequency = input frequence / 510 in phase correcct pwm mode. => 490 / 510 = 0.5Hz
    // TCCR1A |=  _BV(WGM10);

    // Fast PWM Mode. 8 bit resolution. Frequency = input frequence / 510 in phase correcct pwm mode. => 490 / 255 = 2Hz
    TCCR1A |= _BV(WGM12) | _BV(WGM10);

    // Inverting. COM1A1:1 COM1A0:1
    // TCCR1A |= _BV(COM1A1) | _BV(COM1A0);

    // OC1A is output on Port B Pin 1
    DDRB |= _BV(PB1);
    // Non-Inverting. COM1A1:1 COM1A0:0
    TCCR1A |=  _BV(COM1A1);
    TCCR1A &= ~_BV(COM1A0);

    // OC1B is output on Port B Pin 2
    DDRB |= _BV(PB2);
    // Non-Inverting. COM1B1:1 COM1B0:0
    TCCR1A |=  _BV(COM1B1);
    TCCR1A &= ~_BV(COM1B0);
}

int main()
{
    // init AVR
    cli();
    _delay_ms(0);

    // init LCD Display
    lcd_init(LCD_DISP_ON);
    lcd_print_two_lines("LCD Display init", "done");

    // A/D Converter on Port C Pin 3 and 4
    init_adc();
    lcd_print_two_lines("ADC init", "done");
    _delay_ms(1000);

    // // PWM Signal on output OC1
    init_pwm_counter1();
    init_pwm_counter2();

    for (;;)
    {

        // DDRB |= _BV(PB2);
        // PORTB |= _BV(PB2);
        // _delay_ms(500);
        // PORTB &= ~_BV(PB2);
        // _delay_ms(500);

        lcd_clrscr();
        _delay_ms(500);

        // Do A/D conversion on ADC2. Sollwert
        select_channel_adc2();
        uint16_t value_adc2 = convert_atod();
        // A value of 0 -> 10 degrees, A value of 1024 -> 35 degrees
        // all temperatures in 10th of degrees
        uint32_t tmp = ( (uint32_t)value_adc2 * (uint32_t)250 / (uint32_t)1024 ) + 100;
        uint16_t tempSoll = tmp;
        lcd_print_two_uint16s("value_adc2", value_adc2, "tempSoll", tempSoll);
        // _delay_ms(1000);

        // Do A/D conversion on ADC3. Input voltage
        select_channel_adc3();
        uint16_t value_adc3 = convert_atod();
        // 42V -> 2.21V
        // 2.21V = 2.56 * x / 1024 -> x = 884
        uint16_t v0 = value_adc3 * (uint16_t)42 / (uint16_t)884;
        // lcd_print_two_uint16s("value_adc3", value_adc3, "v0", v0);
        // _delay_ms(1000);

        // Do A/D conversion on ADC4. Cooler  temperature
        select_channel_adc4();
        uint16_t value_adc4 = convert_atod();
        uint16_t tempCooler = getRNP50UTempFromAdcValue(value_adc4);

        lcd_print_two_uint16s("ADC4", value_adc4, "TempCooler", tempCooler);
        // _delay_ms(2000);

        // Do A/D conversion on ADC5. Air temperature
        select_channel_adc5();
        uint16_t value_adc5 = convert_atod();
        uint16_t tempAir = getRNP50UTempFromAdcValue(value_adc5);
        lcd_print_two_uint16s("value_adc5", value_adc5, "TempAir", tempAir);

        lcd_print_two_uint16s("TempSoll", tempSoll, "TempAir", tempAir);
        _delay_ms(1000);
        // Calculation of max power due to input voltage
        uint16_t Pmax = v0 * v0 / POWERRESISTOR;

        // Calculation of max power at resistor due to derating curve
        uint16_t PDerating = getRNP50UDeratedPower(tempCooler);
        // lcd_print_two_uint16s("Pmax", Pmax, "PDerating", PDerating);
        // _delay_ms(2000);

        // Calculation of PWM rate
        // Proportional controller
        uint16_t pSollTenth = (tempSoll - tempAir) * 5;
        lcd_print_two_uint16s("pSoll vor grenze", pSollTenth, "p10th", pSollTenth);
        // _delay_ms(1000);
        if (pSollTenth > PDerating * 10)
        {
            pSollTenth = PDerating * 10;
        }
        if (tempAir > tempSoll)
        {
            pSollTenth = 0;
        }
        lcd_print_two_uint16s("pSoll10th", pSollTenth, "pmax", Pmax);
        // _delay_ms(5000);
        // uint32_t pSoll1000 = (tempSoll - tempAir) * P0;
        // uint16_t pSoll_16 = pSoll1000 / 1000;
        uint32_t ptmp = (uint32_t)254 * (uint32_t)pSollTenth / (Pmax * 10);
        uint16_t pwmRate = (uint16_t)ptmp;
        lcd_print_two_uint16s("pSoll", pSollTenth, "pwmRate", pwmRate);
        // _delay_ms(5000);
        if (pwmRate > 254)
        {
            pwmRate = 254;
        }
        // all temps in 10th
        if (tempAir > 500)
        {
            pwmRate = 0;
        }
        // all temps in 10th
        if (tempCooler > 700)
        {
            pwmRate = 0;
        }
        // uint32_t pwmRate = 254 * pSoll1000 / Pmax;
        // uint16_t pwmRate16 = pwmRate / 1000;
        lcd_print_two_uint16s("pSoll nach", pSollTenth, "pwmRate", pwmRate);
        // _delay_ms(1000);
        // OCR1A is PWM rate of heating
        // todo OCR1A = pwmRate;
        OCR1A = 0;
        // OXR1B is for LED showing heating rate
        if (pwmRate > 0)
        {
            OCR1B = pwmRate;
        }
        else
        {
            // maintain minimum brightness 
            OCR1B = 1;
        }
        


        /*
        static uint16_t x = 0;
        if ( ++x > 254)
        {
            x = 0;
       

        // for (uint16_t x = 0; x <= 250; x = x + 2)
        // {
            char buf[16 + 1];
            snprintf(buf, 16 + 1, "to %d", x);
            lcd_print_two_lines("Setting OCR1A", buf);
            lcd_print_two_uint16s("Setting OCA", buf, "PWM rate %", x * (uint16_t)100 / 255);
            // OCR1A is PWM rate of heating
            OCR1A = x;
            // OXR1B is for LED showing heating rate
            OCR1B = x;
            _delay_ms(1000);
        // }
        */
    }
}
