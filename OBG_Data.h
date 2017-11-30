/**
  OBG_Data.h

  This is the a header file of the AVR Butterfly
  application board adaptation of the One Bit Groovebox.
*/

#ifndef OBG_DATA_H
#define OBG_DATA_H

#include <stdint.h>

#include <avr/pgmspace.h>

extern uint16_t Waveforms[64] PROGMEM;
extern uint16_t Frequency_Divider[64] PROGMEM;
extern uint8_t Interval_Vs[128] PROGMEM;
extern uint8_t Power_Reference[16] PROGMEM;
extern uint16_t Noise_Hold_Table[8] PROGMEM;

/** How many bits in waveform. */
#define WAVEFORM_BITS 11

/** Load frequency divider according
    to the sampling rate (platform-dependent) -- W.B. */
#define LOAD_FREQUENCY_DIVIDER(x)				\
  (pgm_read_word(&Frequency_Divider[(x)]) >> 2)

#endif
