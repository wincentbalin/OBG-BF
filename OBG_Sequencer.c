/**
  OBG_Sequencer.c

  This is the a source file of the AVR Butterfly
  application board adaptation of the One Bit Groovebox.

  Here are the definitions of the sequencer
  for the One Bit Groovebox.
*/

#include <stdint.h>

#include "Display.h"			/* To display beat (quarters). */
#include "Timer.h"				/* For setting sequencer timer. */
#include "OBG_Sequencer.h"
#include "OBG_Synthesizer.h"	/* Communicate with synthesizer. */
#include "OBG_Tempo.h"			/* Calculate tempo. */

/** Current beat. */
uint8_t current_beat = 0;

/** Note parameters grid. */
uint8_t note_grid[BEATS][NOTE_PARAMETERS];

/** Set current beat and load accordant parameters. */
void Set_Current_Beat(uint8_t beat)
{
  /* Set current beat. */
  current_beat = beat;
  /* Load it's parameters. */
  Load_Current_Note_Parameters();
}

/** Set OBG tempo. */
void Set_Tempo(uint32_t new_tempo)
{
  uint64_t freq_converter;
  uint16_t tempo_value;
  
  /* Convert frequencies of timers. */
  freq_converter = new_tempo * F_SEQ_TIMER;
  freq_converter = freq_converter / F_TAP_TIMER;

  /* Calculate value and rest of the tempo.
     To do this, divide the interval for quarter
	 through the number of beats in quarter (here 32 / 4 = 8). */
  tempo_value = freq_converter / (BEATS / 4);

  /* Reset Timer 1. */
  Set_Timer1_Value(0);
  /* Set Timer 1 output compare value. */
  Set_Timer1_OC_Value(tempo_value);
}

/** Load current note parameters. */
void Load_Current_Note_Parameters(void)
{
  /* Set parameters of the current note. */
  Set_Current_Waveform(note_grid[current_beat][WAVEFORM]);
  Set_Current_Divider(note_grid[current_beat][DIVIDER]);
  Set_Current_EffParam1(note_grid[current_beat][EFFPARAM1]);
  Set_Current_EffParam2(note_grid[current_beat][EFFPARAM2]);
}

/** Save note parameter. */
void Save_Current_Note_Parameter(note_parameters_t parameter, uint8_t value)
{
  note_grid[current_beat][parameter] = value;
}

/** OBG Sequencer initialization. */
void OBG_Sequencer_Init(void)
{
  uint8_t i;

  /* Set default tempo to 120 BPM. */
  Set_Tempo((F_TAP_TIMER * 60) / 120);

  /* set eff param #2 to non-zero so buttons 5 and 6 have immed. effect. -- N.V. */
  for(i = 0; i < BEATS; i++)
  {
    note_grid[i][EFFPARAM2] = 64;
  }
}

/** OBG sequencer task. */
void OBG_Sequencer_Task(void)
{
  /* Load current note parameters. */
  Load_Current_Note_Parameters();

  /* Process pressed keys. */
  OBG_Keys();

  /* Advance pointer. */
  if(current_beat < BEATS-1)
  {
    current_beat++;
  }
  else
  {
    current_beat = 0;
  }
}
