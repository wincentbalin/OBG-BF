/**
  Display.h

  This is the a header file of the AVR Butterfly
  application board adaptation of the One Bit Groovebox.
*/

#ifndef OBG_BF_DISPLAY_H
#define OBG_BF_DISPLAY_H

#include <stdint.h>

#include "Menu.h"

/** Define type of flags for the display. */
typedef struct
{
  unsigned int label : 1;
  unsigned int tempo : 1;
  unsigned int value : 1;
}
display_flags_t;

/** Define type of the state of the display. */
typedef enum
{
  LABEL,
  TEMPO,
  VALUE
}
display_state_t;

/* Function prototypes. */
void Display_Label(char*);
void Display_Tempo(uint32_t, uint32_t);
void Display_Value(synth_button_t, uint8_t);
void Display_Init(void);
void Display_Task(void);

#endif
