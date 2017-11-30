/**
  ADC.h

  This is the a header file of the AVR Butterfly
  application board adaptation of the One Bit Groovebox.
*/

#ifndef OBG_BF_ADC_H
#define OBG_BF_ADC_H

#include <stdint.h>

/** Amount of saved ADC values. */
#define ADC_VALUES_N 8

/** Type definition of the ring buffer for ADC values. */
typedef struct
{
  uint16_t data[ADC_VALUES_N];
  uint16_t* current;
  uint16_t* end;
}
adc_ring_buffer_t;

/* Prototypes of functions. */
void ADC_Init(void);
uint8_t ADC_Result(void);

#endif
