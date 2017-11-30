/**
  OBG_Tempo.c

  This is the a source file of the AVR Butterfly
  application board adaptation of the One Bit Groovebox.

  Here are the definitions of the tempo calculator
  for the One Bit Groovebox.
*/

#include "Display.h"		/* For LCD output. */
#include "Timer.h"
#include "OBG_Sequencer.h"	/* Tempo data type, Set_Tempo() et cetera. */
#include "OBG_Tempo.h"

/** Current value of calculated tempo. */
uint32_t current_tempo;

/** Tempo calculator state. */
tempo_calc_state_t tempo_calc_state = TAP1;

/** Timestamp (set when tempo was tapped). */
uint32_t timestamp;

/** Current tap timestamp. */
uint32_t current_tap_timestamp;

/** Previous tap timestamp. */
uint32_t previous_tap_timestamp;

/** Set timestamp (called externally). */
void Set_Tap_Timestamp(uint32_t tap_timestamp)
{
  timestamp = tap_timestamp;
}

/** OBG tempo calculator task. */
void OBG_Tempo_Task(void)
{
  uint32_t interval;

  if(tempo_calc_state == TAP1)
  {
    /* Get first timestamp. */
    previous_tap_timestamp = timestamp;
  }
  else
  {
    /* Get current timestamp. */
    current_tap_timestamp = timestamp;

    /* Calculate interval between two taps. */
	interval = Calculate_Timestamp_Interval(previous_tap_timestamp,
	                                        current_tap_timestamp);

	/* Check if there is an apparent glitch. */
	if(interval < MAX_TEMPO_PULSES)
	{
	  return;
	}

	/* Check if someone forgot to complete four taps sequence. */
	if(interval > MIN_TEMPO_PULSES)
	{
	  /* Count this tap as the first one. */
	  tempo_calc_state = TAP1;
	  OBG_Tempo_Task();
      return;
	}

	/* Depending on state, calculate new tempo. */
	switch(tempo_calc_state)
	{
	  case TAP2:
	    current_tempo = interval;
	    break;

	  case TAP3:
	    current_tempo = (current_tempo + interval) / 2;
	    break;

	  case TAP4:
	    current_tempo = (current_tempo * 2 + interval) / 3;
	    break;

      default:
	    break;
	}

	/* If preliminary tempo calculated, display it. */
	if(tempo_calc_state != TAP1)
	{
	  Display_Tempo(current_tempo, F_TAP_TIMER);
	}

	/* If tempo calculated, set it. */
	if(tempo_calc_state == TAP4)
	{
	  Set_Tempo(current_tempo);
	}

    /* Backup current timestamp. */
    previous_tap_timestamp = current_tap_timestamp;
  }

  /* Set current beat to the corresponding quarter and
     transit to the next state. */
  switch(tempo_calc_state)
  {
    case TAP1:
      Set_Current_Beat((BEATS / 4) * (1-1));
	  tempo_calc_state = TAP2;
	  break;

    case TAP2:
      Set_Current_Beat((BEATS / 4) * (2-1));
	  tempo_calc_state = TAP3;
	  break;

    case TAP3:
      Set_Current_Beat((BEATS / 4) * (3-1));
	  tempo_calc_state = TAP4;
	  break;

    case TAP4:
      Set_Current_Beat((BEATS / 4) * (4-1));
	  tempo_calc_state = TAP1;
	  break;
  }
}
