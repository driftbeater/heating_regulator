#ifndef NTC_B7703M103G40_INCL_
#define NTC_B7703M103G40_INCL_

#include <avr/io.h>

uint16_t getRNP50UDeratedPower(uint16_t tempMilli);

// Temperate based on the r-value of the resistor
// Temperature is in 10th of degrees
uint16_t getRNP50UTempFromAdcValue(uint16_t adcValue);

#endif