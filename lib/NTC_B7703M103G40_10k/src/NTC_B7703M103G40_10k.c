#include "NTC_B7703M103G40_10k.h"

#include <avr/io.h>


uint16_t getRNP50UDeratedPower(uint16_t temp)
{
  // Maximum power dependent on flange temperatur
  // temperatures in 10th of degrees
  int16_t pmax = (int16_t)58 - ( temp / 30 );
  if (pmax < 0)
  { 
    return 0;
  } 
  if (pmax > 50) 
  { 
      return 50;
  }
  return pmax;
}

// Temperature is in 10th degree
uint16_t getRNP50UTempFromAdcValue(uint16_t adcValue)
{
  // adc values for temperatures from 4 to 70 degrees.
    static uint16_t temp[67] = {
940,
915,
882,
850,
820,
789,
760,
731,
706,
678,
655,
630,
613,
592,
576,
558,
541,
527,
513,
500,
486,
473,
459,
442,
430,
419,
405,
394,
379,
367,
354,
341,
332,
320,
312,
302,
292,
283,
272,
261,
251,
240,
230,
220,
214,
209,
203,
195,
192,
186,
180,
173,
168,
163,
158,
153,
148,
140,
130,
128,
125,
115,
110,
105,
100,
95,
90
};

    // The smallest temperature will be 4 degrees.
    // The biggest temperature will be 70 degrees.
    // if adcvalue is smaller than 90, the temperature returned will be 255 degree, i.e. 2550 10-th 
    for (int x =0; x < 67; ++x)
    {
      if (adcValue > temp[x])
      {
        uint16_t temperature = (x + 4) * 10;
        // interpolation between the configured values
        uint16_t delta = adcValue -temp[x];
        if (delta > 9)
        {
          delta = 9;
        }
        temperature = temperature + delta;
        return temperature;
      }
    } 
    
    return (uint16_t)2550;
}