/**
  OBG_Sequencer.h

  This is the a header file of the AVR Butterfly
  application board adaptation of the One Bit Groovebox.
*/

#ifndef OBG_SEQUENCER_H
#define OBG_SEQUENCER_H

#include <stdint.h>

/** Beats per measure. */
#define BEATS 32

/** Definition of the type of note parameters. */
typedef enum
{
  WAVEFORM = 0,
  DIVIDER,
  EFFPARAM1,
  EFFPARAM2,
  NOTE_PARAMETERS
}
note_parameters_t;

/** Frequency of the sequencer timer.

    Very hardware-dependent.
	In this case F_CPU / TIMER1_PRESCALER = 8000000 / 64 = 125000 Hz.
*/
#define F_SEQ_TIMER (8000000 / 64)

/* Function prototypes. */
void Set_Current_Beat(uint8_t);
void Set_Tempo(uint32_t);
void Load_Current_Note_Parameters(void);
void Save_Current_Note_Parameter(note_parameters_t, uint8_t);
void OBG_Sequencer_Init(void);
void OBG_Sequencer_Task(void);

#endif
