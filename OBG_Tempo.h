/**
  OBG_Tempo.h

  This is the a header file of the AVR Butterfly
  application board adaptation of the One Bit Groovebox.
*/

#ifndef OBG_TEMPO_H
#define OBG_TEMPO_H

#include <stdint.h>

/** Minimal tempo. */
#define MIN_TEMPO 40

/** Maximal tempo. */
#define MAX_TEMPO 240

/** Frequency of the tap tempo timer (look up "alliteration").

    In this case 32768 Hz.
*/
#define F_TAP_TIMER 32768

/** Minimal tempo timer pulses for one quarter. */
#define MIN_TEMPO_PULSES ((60 * F_TAP_TIMER) / MIN_TEMPO)

/** Maximal tempo timer pulses for one quarter. */
#define MAX_TEMPO_PULSES ((60 * F_TAP_TIMER) / MAX_TEMPO)

/** Type definition of the tempo calculator state. */
typedef enum
{
  TAP1,
  TAP2,
  TAP3,
  TAP4
}
tempo_calc_state_t;

/* Function prototypes. */
void Set_Tap_Timestamp(uint32_t);
void OBG_Tempo_Task(void);

#endif
