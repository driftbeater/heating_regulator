#ifndef NTC_B7703M103G40_INCL_
#define NTC_B7703M103G40_INCL_

#include <avr/io.h>

uint16_t getRNP50UDeratedPower(uint16_t tempMilli);
uint16_t getRNP50UTempFromAdcValue(uint16_t adcValue);

#endif