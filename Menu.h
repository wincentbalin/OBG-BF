/**
  Menu.h

  This is the a header file of the AVR Butterfly
  application board adaptation of the One Bit Groovebox.
*/

#ifndef OBG_BF_MENU_H
#define OBG_BF_MENU_H

#include <avr/pgmspace.h>

/** Define joystick value type. */
typedef enum
{
  KEY_NULL = 0,
  KEY_ENTER,
  KEY_NEXT,
  KEY_PREV,
  KEY_PLUS,
  KEY_MINUS
}
joystick_t;

/**
  Define macro for joystick port pins
  Taken over from the original AVR firmware -- W.B. */
#define PINB_MASK ((1<<PINB4)|(1<<PINB6)|(1<<PINB7))
#define PINE_MASK ((1<<PINE2)|(1<<PINE3))

/** Values of joytick button pins. */
#define BUTTON_A    6   // UP
#define BUTTON_B    7   // DOWN
#define BUTTON_C    2   // LEFT
#define BUTTON_D    3   // RIGHT
#define BUTTON_O    4   // PUSH

/** Synthesizer button type. */
typedef enum
{
  BUTTON_WAVEFORM_JAM = 0,
  BUTTON_WAVEFORM_RECORD,
  BUTTON_PITCH_JAM,
  BUTTON_PITCH_RECORD,
  BUTTON_EFFPARAM1_JAM,
  BUTTON_EFFPARAM1_RECORD,
  BUTTON_EFFPARAM2_JAM,
  BUTTON_EFFPARAM2_RECORD,
  BUTTON_EFFECT_MODE,
  BUTTON_TAP_TEMPO,
  BUTTONS_N
}
synth_button_t;

/** Menu type. */
typedef struct PROGMEM
{
  synth_button_t id;
  PGM_P label;
  void (*function)(void);
  synth_button_t up;
  synth_button_t right;
  synth_button_t down;
  synth_button_t left;
}
menu_t;

/* Define macro to make menu creation easier. */
#define MENUPOINT(BUTTON, FUNCTION, UP, RIGHT, DOWN, LEFT) \
  {                                                        \
    BUTTON_ ## BUTTON,                                     \
	LABEL_ ## BUTTON,                                      \
	FUNCTION,                                              \
	BUTTON_ ## UP,                                         \
	BUTTON_ ## RIGHT,                                      \
	BUTTON_ ## DOWN,                                       \
	BUTTON_ ## LEFT                                        \
  }

/* Taken over from the original AVR firmware -- W.B. */
#ifndef FALSE
#define FALSE   0
#define TRUE    (!FALSE)
#endif

/* Prototypes of functions. */
void Menu_Init(void);
void Menu_Task(void);
joystick_t Get_Joystick(void);
synth_button_t Get_Button(void);

#endif
