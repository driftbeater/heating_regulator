

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

// 30 Ohm
const uint16_t POWERRESISTOR = 30;
// Max temperature of resistor is 250 degrees
const uint16_t tempResistorMax10th = 2500;
const uint16_t pMaxResistor10th = 500;

// temperatur is in 10th of degrees
// power is in 10th of watts
uint16_t getDeratedPower(uint16_t temp10th)
{
  // Maximum power dependent on flange temperatur
  // temperatures in 10th of degrees
  // power in 10th of watts
  // maximum power at 0 degrees, degrades to 0 at 250 degrees
  int16_t pDerated10th = (int16_t)pMaxResistor10th * ( 1 - temp10th / tempResistorMax10th );
  if (pDerated10th < 0)
  { 
    return 0;
  }
  if (temp10th > 600)
  {
    return 0;
  }
  if (temp10th > 550 && pDerated10th > 100)
  {
    return 100;
  }
  if (temp10th > 500 && pDerated10th > 200)
  {
    return 200;
  }
  if (temp10th > 450 && pDerated10th > 300)
  {
    return 400;
  }
  if (pDerated10th > pMaxResistor10th) 
  { 
      return pMaxResistor10th;
  }

 return pDerated10th;
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
    // Do conversion 3 times. Discard first measurment and return average of subsequent two.

    // Start Conversion
    ADCSRA |= _BV(ADSC);
    loop_until_bit_is_clear(ADCSRA, ADSC);
    uint16_t value = ADCW;

    ADCSRA |= _BV(ADSC);
    loop_until_bit_is_clear(ADCSRA, ADSC);
    value = ADCW;

    ADCSRA |= _BV(ADSC);
    loop_until_bit_is_clear(ADCSRA, ADSC);
    value += ADCW;

    return value / 2;
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
    /// todo lcd lcd_init(LCD_DISP_ON);
    /// todo lcd lcd_print_two_lines("LCD Display init", "done");
    /// todo lcd _delay_ms(1000);

    // A/D Converter on Port C Pin 3 and 4
    init_adc();
    /// todo lcd lcd_print_two_lines("ADC init", "done");
    _delay_ms(1000);

    // // PWM Signal on output OC1
    init_pwm_counter1();
    init_pwm_counter2();

    for (;;)
    {
        /// todo lcd lcd_clrscr();
        //_delay_ms(500);

        // Do A/D conversion on ADC2. Sollwert
        select_channel_adc2();
        uint16_t value_adc2 = convert_atod();
        // A value of 0 -> 10 degrees, A value of 1024 -> 35 degrees
        // all temperatures in 10th of degrees
        uint32_t tmp = ( (uint32_t)value_adc2 * (uint32_t)250 / (uint32_t)1024 ) + 100;
        uint16_t tempSoll10th = tmp;
        /// todo lcd lcd_print_two_uint16s("value_adc2", value_adc2, "tempSoll", tempSoll10th);
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
        uint16_t tempCooler10th = getRNP50UTempFromAdcValue(value_adc4);

        /// todo lcd lcd_print_two_uint16s("ADC4", value_adc4, "TempCooler", tempCooler10th);
        /// todo lcd _delay_ms(1000);
        // Do A/D conversion on ADC5. Air temperature
        select_channel_adc5();
        uint16_t value_adc5 = convert_atod();
        uint16_t tempAir10th = getRNP50UTempFromAdcValue(value_adc5);
        /// todo lcd lcd_print_two_uint16s("value_adc5", value_adc5, "TempAir", tempAir10th);

        /// todo lcd lcd_print_two_uint16s("TempSoll", tempSoll10th, "TempAir", tempAir10th);
        /// todo lcd _delay_ms(1000);
        // Calculation of max power due to input voltage
        uint16_t Pmax = v0 * v0 / POWERRESISTOR;

        // Calculation of max power at resistor due to derating curve
        uint16_t PDerating10th = getDeratedPower(tempCooler10th);
        // lcd_print_two_uint16s("Pmax", Pmax, "PDerating_tenth", PDerating_tenth);
        // _delay_ms(2000);

        // Calculation of PWM rate
        // Proportional controller
        int16_t pTmp = (tempSoll10th - tempAir10th) * 10;
        uint16_t pSollTenth = pTmp > 0 ? pTmp : 0;
        /// todo lcd lcd_print_two_uint16s("pSoll vor grenze", pSollTenth, "p10th", pSollTenth);
        // _delay_ms(1000);
        if (pSollTenth > PDerating10th)
        {
            pSollTenth = PDerating10th;
        }
        if (PDerating10th > pMaxResistor10th)
        {
            pSollTenth = pMaxResistor10th;
        }
        if (tempAir10th > tempSoll10th)
        {
            pSollTenth = 0;
        }
        if (tempAir10th > 500)
        {
            pSollTenth = 0;
        }
        if (tempCooler10th > 600)
        {
            pSollTenth = 0;
        }
        /// todo lcd lcd_print_two_uint16s("pSoll10th", pSollTenth, "pmax", Pmax);
        // _delay_ms(5000);

        uint32_t pwmRate32 = (uint32_t)254 * (uint32_t)pSollTenth / (Pmax * 10);
        uint16_t pwmRate = (uint16_t)pwmRate32;
        /// todo lcd lcd_print_two_uint16s("pSoll", pSollTenth, "pwmRate", pwmRate);
        // _delay_ms(5000);
        if (pwmRate > 254)
        {
            pwmRate = 254;
        }

        // uint32_t pwmRate = 254 * pSoll1000 / Pmax;
        // uint16_t pwmRate16 = pwmRate / 1000;
        /// todo lcd lcd_print_two_uint16s("pSoll nach", pSollTenth, "pwmRate", pwmRate);
        /// todo lcd _delay_ms(1000);
        // OCR1A is PWM rate of heating
        OCR1A = pwmRate;
        
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

        _delay_ms(3000);
    }
}
