/**
  Speaker.c

  This is the a source file of the AVR Butterfly
  application board adaptation of the One Bit Groovebox.

  Here go the functions for initialization of and output on speaker.
*/

#include <avr/io.h>

#include "Speaker.h"

/** Initialize speaker. */
void Speaker_Init(void)
{
  /* Initialize pin 5 on port B (on AVR Butterfly) as output,
     as pin3 too, because it is located near the speaker,
	 and also because all other pins nearby are used on joystick. */
  DDRB |= (1 << DDB5 | 1 << DDB3);
  /* Pin 3 of port is the output ground, so zero it.
	 Zero pin 5 (speaker) too to make it silent. */
  PORTB &= ~(1 << PB5 | 1 << PB3);
}

/** Output (on) speaker. */
void Speaker_Output(uint8_t value)
{
  if(value == 0)
  {
    PORTB &= ~(1 << PB5);
  }
  else
  {
     PORTB |= (1 << PB5);
  }
}
