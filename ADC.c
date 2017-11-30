/**
  ADC.c

  This is the a source file of the AVR Butterfly
  application board adaptation of the One Bit Groovebox.

  This file provides means to read values from ADC.
*/

#include <avr/io.h>
#include <avr/interrupt.h>

#include "ADC.h"

/** Buffer for voltage values. */
adc_ring_buffer_t voltage;

/** Initialization of a ADC ring buffer. */
void adc_ring_buffer_init(adc_ring_buffer_t* b)
{
  b->current = b->data;
  b->end = b->data+ADC_VALUES_N-1;
}

/** Write new value to ADC ring buffer. */
void adc_ring_buffer_write(adc_ring_buffer_t* b, uint16_t value)
{
  /* Write value. */
  *(b->current) = value;

  /* Advance pointer. */
  if(b->current == b->end)
  {
    b->current = b->data;
  }
  else
  {
    b->current++;
  }
}

/** Get mean of all values in the ADC ring buffer. */
uint16_t adc_ring_buffer_mean(adc_ring_buffer_t* b)
{
  uint8_t i;
  uint16_t sum = 0;

  for(i = 0; i < ADC_VALUES_N; i++)
  {
    sum += b->data[i];
  }

  return(sum / ADC_VALUES_N);
}

/** Initialization of ADC. */
void ADC_Init(void)
{
  /* Disable JTAG. */
  MCUCR = 1 << JTD;
  MCUCR = 1 << JTD;

  /* Initialize ADC ring buffers. */
  adc_ring_buffer_init(&voltage);

  /* Internal 1.1V reference voltage, voltage measuring input ADC1. */
  ADMUX = 1 << REFS1 | 1 << REFS0 | 1 << MUX0;

  /* Enable ADC interrupt, prescaler with F_CPU / 128.
     Enable ADC and start free-running conversions. */
  ADCSRA = 1 << ADEN | 1 << ADSC | 1 << ADATE | 1 << ADIE | 1 << ADPS2 | 1 << ADPS1 | 1 << ADPS0;
}

/** ADC ISR. */
ISR(ADC_vect)
{
  adc_ring_buffer_write(&voltage, ADCW);
}

/** ADC result. */
uint8_t ADC_Result(void)
{
  return adc_ring_buffer_mean(&voltage) >> 1;
}
