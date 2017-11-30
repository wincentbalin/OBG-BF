/**
  OBG_Synthesizer.h

  This is the a header file of the AVR Butterfly
  application board adaptation of the One Bit Groovebox.
*/

#ifndef OBG_SYNTHESIZER_H
#define OBG_SYNTHESIZER_H

#include <stdint.h>

/** Type definition of the playing waveform. */
typedef struct
{
  uint16_t loaded;
  uint16_t playing;
  uint8_t index;
}
loaded_waveform_t;

/** Type definitions of effect modes. */
typedef enum
{
  DRY = 0,
  COMB,
  RING,
  COMBCRAZY,
  ARP,
  CHORD,
  TREM,
  DUAL,
  EFFECTS
}
effect_t;

/* Function prototypes. */
effect_t Get_Effect(void);
void Set_Current_Waveform(uint8_t);
void Set_Current_Divider(uint8_t);
void Set_Current_EffParam1(uint8_t);
void Set_Current_EffParam2(uint8_t);
void OBG_Keys(void);
void OBG_Synthesizer_Init(void);
void OBG_Synthesizer_Task(void);

#endif
