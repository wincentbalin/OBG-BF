/**
  Menu.C

  This is the a source file of the AVR Butterfly
  application board adaptation of the One Bit Groovebox.

  Both the menu and the functions around it are defined here.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "Menu.h"
#include "Display.h"			/* For printing menu label on LCD. */
#include "Timer.h"				/* For triggering tempo taps. */
#include "OBG_Synthesizer.h"	/* For setting parameters. */

/* Taken over from button.c of the originl AVR firmware -- W.B. */
volatile unsigned char gButtonTimeout = FALSE;

/** Value of joystick. */
volatile joystick_t key = KEY_NULL;

/** Old value of joystick. */
volatile joystick_t key_old = KEY_NULL;

/** Validness flag of joystick. */
volatile uint8_t key_new = 0;

/** Current synthesizer button. */
volatile synth_button_t button_current = BUTTONS_N;

/** Pressed button. */
volatile synth_button_t button_pressed = BUTTONS_N;

/* Labels of menu buttons. */
const char LABEL_WAVEFORM_JAM[] PROGMEM = "WAVEFORM JAM";
const char LABEL_WAVEFORM_RECORD[] PROGMEM = "WAVEFORM RECORD";
const char LABEL_PITCH_JAM[] PROGMEM = "PITCH JAM";
const char LABEL_PITCH_RECORD[] PROGMEM = "PITCH RECORD";
const char LABEL_EFFPARAM1_JAM[] PROGMEM = "EFFPARAM1 JAM";
const char LABEL_EFFPARAM1_RECORD[] PROGMEM = "EFFPARAM1 RECORD";
const char LABEL_EFFPARAM2_JAM[] PROGMEM = "EFFPARAM2 JAM";
const char LABEL_EFFPARAM2_RECORD[] PROGMEM = "EFFPARAM2 RECORD";
const char LABEL_EFFECT_MODE[] PROGMEM = "EFFECT MODE";
const char LABEL_TAP_TEMPO[] PROGMEM = "TAP TEMPO";

/*
  Menu consists of:

  +----------------------+-------------------------+
  | BUTTON_WAVEFORM_JAM  | BUTTON_WAVEFORM_RECORD  |
  | BUTTON_PITCH_JAM     | BUTTON_PITCH_RECORD     |
  | BUTTON_EFFPARAM1_JAM | BUTTON_EFFPARAM1_RECORD |
  | BUTTON_EFFPARAM2_JAM | BUTTON_EFFPARAM2_RECORD |
  | BUTTON_EFFECT_MODE   | BUTTON_TAP_TEMPO        |
  +----------------------+-------------------------+
*/

/** Empty menu function stub. */
void empty(void)
{
}

/** Menu structure. */
const menu_t menu[] PROGMEM =
{
  MENUPOINT(WAVEFORM_JAM, empty, EFFECT_MODE, WAVEFORM_RECORD, PITCH_JAM, WAVEFORM_RECORD),
  MENUPOINT(WAVEFORM_RECORD, empty, TAP_TEMPO, WAVEFORM_JAM, PITCH_RECORD, WAVEFORM_JAM),
  MENUPOINT(PITCH_JAM, empty, WAVEFORM_JAM, PITCH_RECORD, EFFPARAM1_JAM, PITCH_RECORD),
  MENUPOINT(PITCH_RECORD, empty, WAVEFORM_RECORD, PITCH_JAM, EFFPARAM1_RECORD, PITCH_JAM),
  MENUPOINT(EFFPARAM1_JAM, empty, PITCH_JAM, EFFPARAM1_RECORD, EFFPARAM2_JAM, EFFPARAM1_RECORD),
  MENUPOINT(EFFPARAM1_RECORD, empty, PITCH_RECORD, EFFPARAM1_JAM, EFFPARAM2_RECORD, EFFPARAM1_JAM),
  MENUPOINT(EFFPARAM2_JAM, empty, EFFPARAM1_JAM, EFFPARAM2_RECORD, EFFECT_MODE, EFFPARAM2_RECORD),
  MENUPOINT(EFFPARAM2_RECORD, empty, EFFPARAM1_RECORD, EFFPARAM2_JAM, TAP_TEMPO,EFFPARAM2_JAM),
  MENUPOINT(EFFECT_MODE, empty, EFFPARAM2_JAM, TAP_TEMPO, WAVEFORM_JAM, TAP_TEMPO),
  MENUPOINT(TAP_TEMPO, Timer2_Trigger, EFFPARAM2_RECORD, EFFECT_MODE, WAVEFORM_RECORD, EFFECT_MODE)
};

/** Pointer to the current menu point. */
uint8_t current_menu_point;

/** Find pointer to the current menu point. */
void Find_Menu_Point(synth_button_t button)
{
  uint8_t i;

  for(i = 0; i < BUTTONS_N; i++)
  {
    if(pgm_read_byte(&menu[i].id) == button)
	{
	  current_menu_point = i;

	  button_current = button;

	  Display_Label((PGM_P) pgm_read_word(&menu[current_menu_point].label));

	  return;
	}
  }
}

/**
  Initialize menu infrastructure.

  Assign buttons et cetera.

  Taken over from the original AVR firmware -- W.B.
*/
void Menu_Init(void)
{
  /* Enable pull-up resistors. */
  PORTB = (15<<PB0);
  PORTE = (15<<PE4);

  /* Initialize port pins for joystick. */
  DDRB &= ~(1<<PB7);
  DDRB &= ~(1<<PB6);
  DDRB &= ~(1<<PB4);
  PORTB |= PINB_MASK;
  DDRE = 0x00;
  PORTE |= PINE_MASK;

  /* Enable pin change interrupt on PORTB and PORTE. */
  PCMSK0 = PINE_MASK;
  PCMSK1 = PINB_MASK;
  EIFR = (1<<PCIF0)|(1<<PCIF1);
  EIMSK = (1<<PCIE0)|(1<<PCIE1);

  /* Begin with virtual button TAP TEMPO . */
  Find_Menu_Point(BUTTON_TAP_TEMPO);
}

/**
  Calculate resulting joystick value.

  Called on every pin change.

  Taken over from the original AVR firmware -- W.B.
*/
void KeyPressed(void)
{
  uint8_t buttons;

  /*
    Read the buttons:

    Bit             7   6   5   4   3   2   1   0
    ---------------------------------------------
    PORTB           B   A       O
    PORTE                           D   C
    ---------------------------------------------
    PORTB | PORTE   B   A       O   D   C
    =============================================
  */
  buttons  = (~PINB) & PINB_MASK;
  buttons |= (~PINE) & PINE_MASK;

  /* Calculate resulting key. */
  if(buttons & (1 << BUTTON_A))
    key = KEY_PLUS;
  else if(buttons & (1 << BUTTON_B))
    key = KEY_MINUS;
  else if(buttons & (1 << BUTTON_C))
    key = KEY_PREV;
  else if(buttons & (1 << BUTTON_D))
    key = KEY_NEXT;
  else if(buttons & (1 << BUTTON_O))
    key = KEY_ENTER;
  else
    key = KEY_NULL;

  /* Debounce key. */
  if(key != key_old)
  {
    /* gButtonTimeout is a flag set periodically by the LCD interrupt. */
    if(gButtonTimeout)
	{
	  if(!key_new)
	  {
        key_new = 1;
	  }
	  gButtonTimeout = FALSE;
	}
  }

  /* Find out which button was pressed. */
  button_pressed = (key == KEY_ENTER) ? button_current : BUTTONS_N;

  /* Backup previous value. */
  key_old = key;

  /* Delete pin change interrupt flags. */
  EIFR = (1 << PCIF1) | (1 << PCIF0);
}


/** Pin change 0 -- call PinToJoystick() .*/
ISR(PCINT0_vect)
{
  KeyPressed();
}

/** Pin change 1 -- call PinToJoystick() .*/
ISR(PCINT1_vect)
{
  KeyPressed();
}

/**
  Return (new) value of joystick.

  Taken over from the original AVR firmware -- W.B.
*/
joystick_t Get_Joystick(void)
{
  joystick_t result;

  cli();

  if(key_new)	/* Check for a new key in buffer. */
  {
    result = key;
    key_new = 0;
  }
  else					/* No key stroke. */
  {
    result = KEY_NULL;
  }

  sei();

  return result;
}

/** Get value of a button currently pressed. */
synth_button_t Get_Button(void)
{
  return button_pressed;
}

/**
  Menu task which is called periodically.

  Processes joystick events.
*/
void Menu_Task(void)
{
  joystick_t j = Get_Joystick();

  if(j == KEY_NULL)
  {
    /* Return if no key pressed. */
    return;
  }
  else if(j == KEY_ENTER)
  {
    /* Execute key function. */
    void (*f)(void) = (PGM_VOID_P) pgm_read_word(&menu[current_menu_point].function);
	(*f)();
  }
  else
  {
    /* Change menu point. */
    synth_button_t next_button;

	switch(j)
	{
	  case KEY_PLUS:	// up
	    next_button = pgm_read_byte(&menu[current_menu_point].up);
		break;
      case KEY_NEXT:	// right
	    next_button = pgm_read_byte(&menu[current_menu_point].right);
		break;
      case KEY_MINUS:	// down
	    next_button = pgm_read_byte(&menu[current_menu_point].down);
		break;
      case KEY_PREV:	// left
	    next_button = pgm_read_byte(&menu[current_menu_point].left);
		break;
      default:			// unknown key
	    return;
	}

	Find_Menu_Point(next_button);	// find next menu point
  }
}


