/**
  Speaker.h

  This is the a header file of the AVR Butterfly
  application board adaptation of the One Bit Groovebox.
*/

#ifndef OBG_SPEAKER_H
#define OBG_SPEAKER_H

#include <stdint.h>

/* Function prototypes. */
void Speaker_Init(void);
void Speaker_Output(uint8_t);

#endif
