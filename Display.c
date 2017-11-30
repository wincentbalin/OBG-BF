/**
  Display.c

  This is the a source file of the AVR Butterfly
  application board adaptation of the One Bit Groovebox.

  Here go the functions which abstract display.
*/

#include <stdint.h>

#include "Display.h"
#include "ADC.h"				/* ADC value. */
#include "LCD_driver.h"			/* LCD initialization. */
#include "LCD_functions.h"		/* LCD output. */
#include "Menu.h"				/* Button type. */
#include "Timer.h"				/* Duration of tempo display. */
#include "OBG_Synthesizer.h"	/* Effect type. */
#include "OBG_Sequencer.h"		/* Number of beats et cetera. */
#include "OBG_Tempo.h"			/* Frequency of slow (tap) timer. */

/** Flags for the display. */
display_flags_t display_flags = {0, 0};

/** Display state. */
display_state_t display_state = LABEL;

/** Pointer to label. */
char* display_label = 0;

/** Tempo to show. */
char* display_tempo = "120_00";

/** Timestamp when showing tempo started. */
uint32_t display_tempo_timestamp = 0;

/** Button to display the value of. */
synth_button_t display_button;

/** Value of the button. */
uint8_t display_button_value = 0;

/** Display label. */
void Display_Label(char* new_label)
{
  /* Set new label. */
  display_label = new_label;

  /* Set display flag. */
  display_flags.label = 1;
}

/** Display tempo. */
void Display_Tempo(uint32_t quarter_ticks, uint32_t ticks_frequency)
{
  /* Calculate tempo in BPM, format XXX_XX . */
  /* Multiply ticks frequency with 60 (seconds in minute). */
  uint32_t
  ticks_per_minute = 60 * ticks_frequency;

  uint8_t
  bpm_before_point = ticks_per_minute / quarter_ticks;

  uint32_t
  bpm_before_point_rest = ticks_per_minute % quarter_ticks;

  /* Multiply bpm_before_point_rest with 100. */
  bpm_before_point_rest *= 100;

  uint8_t
  bpm_after_point = bpm_before_point_rest / quarter_ticks;

  /* Calculate hundreds of BPM. */
  uint8_t
  result = bpm_before_point / 100;
  display_tempo[0] = (result > 0) ? ('0'+result) : ' ';
  bpm_before_point %= 100;

  /* Calculate tens of BPM. */
  result = bpm_before_point / 10;
  display_tempo[1] = '0'+result;
  bpm_before_point %= 10;

  /* Calculate ones of BPM. */
  display_tempo[2] = '0'+bpm_before_point;

  /* Calculate tenths of BPM. */
  result = bpm_after_point / 10;
  display_tempo[4] = '0'+result;
  bpm_after_point %= 10;

  /* Calculate hundredths of BPM. */
  display_tempo[5] = '0'+bpm_after_point;

  /* Set display flag. */
  display_flags.tempo = 1;
}

/** Display button value. */
void Display_Value(synth_button_t button, uint8_t value)
{
  /* Store values. */
  display_button = button;
  display_button_value = value;

  /* Set flag. */
  display_flags.value = 1;
}

/** Calculate value of button to display. */
void Calculate_Button_Value(void)
{
  static char* value_label = "   "; /* Needed only for buttons with analog input. */
  uint8_t adc_value;
  uint8_t adc_rest;
  effect_t effect;	/* Needed only for the effect button. */
  char* effect_label;

  switch(display_button)
  {
    case BUTTON_WAVEFORM_RECORD:
	case BUTTON_WAVEFORM_JAM:
	case BUTTON_PITCH_RECORD:
	case BUTTON_PITCH_JAM:
	case BUTTON_EFFPARAM1_RECORD:
	case BUTTON_EFFPARAM1_JAM:
	case BUTTON_EFFPARAM2_RECORD:
	case BUTTON_EFFPARAM2_JAM:
	  adc_value = display_button_value;
	  adc_rest = adc_value % 100;
	  adc_value = adc_value / 100;
      value_label[0] = '0'+adc_value;
	  adc_value = adc_rest / 10;
	  adc_rest = adc_rest % 10;
      value_label[1] = '0'+adc_value;
	  value_label[2] = '0'+adc_rest;
      LCD_puts(value_label, 0);
	  break;

    case BUTTON_EFFECT_MODE:
	  effect = Get_Effect();
	       if(effect == DRY)       effect_label = PSTR("DRY");
	  else if(effect == COMB)      effect_label = PSTR("C FILT");
	  else if(effect == RING)      effect_label = PSTR("RINGM");
	  else if(effect == COMBCRAZY) effect_label = PSTR("CCFILT");
	  else if(effect == ARP)       effect_label = PSTR("ARP");
	  else if(effect == CHORD)     effect_label = PSTR("CHORD");
	  else if(effect == TREM)      effect_label = PSTR("TREM");
	  else if(effect == DUAL)      effect_label = PSTR("DUAL");
	  else                         effect_label = PSTR("");
      LCD_puts_f(effect_label, 1);
      break;

    default:
	  break;
  }
}

/** Initialize display. */
void Display_Init(void)
{
  /* Call LCD initialization. */
  LCD_Init();

  /* Set decimal "point" in tempo. */
  display_tempo[3] = '_';
}

/** Display task which is executed periodically. */
void Display_Task(void)
{
  /* Display label. */
  if(display_flags.label == 1)
  {
    /* Show label. */
    LCD_puts_f(display_label, 1);
	/* Set state. */
	display_state = LABEL;
	/* Reset flag. */
	display_flags.label = 0;
  }

  /* Display tempo. */
  if(display_flags.tempo == 1)
  {
    /* Show tempo. */
    LCD_puts(display_tempo, 0);
	/* Set display tempo timestamp. */
	display_tempo_timestamp = Get_Timestamp();
	/* Set state. */
	display_state = TEMPO;
	/* Reset flag. */
    display_flags.tempo = 0;
  }

  /* Display value. */
  if(display_flags.value == 1)
  {
    Calculate_Button_Value();
	/* Set state. */
	display_state = VALUE;
	/* Reset flag. */
	display_flags.value = 0;
  }

  /* Check if showing tempo expired (after 1.5 seconds).
     If so, change back to label. */
  if(display_state == TEMPO)
  {
    if(Calculate_Current_Timestamp_Interval(display_tempo_timestamp) >=
	                                                ((F_TAP_TIMER / 2) * 3))
	{
	  display_flags.label = 1;
	}
  }
  /* Check if no button pressed and in button value state,
     then show label again. */
  else if(display_state == VALUE)
  {
    synth_button_t current_button = Get_Button();
    if(current_button == BUTTONS_N || current_button == BUTTON_TAP_TEMPO)
	{
	  display_flags.label = 1;
	}
  }
}
