/**
  Clock.h

  This is the a header file of the AVR Butterfly
  application board adaptation of the One Bit Groovebox.
*/

#ifndef OBG_BF_CLOCK_H
#define OBG_BF_CLOCK_H

/* Prototypes of functions. */
void Delay(unsigned int);
void OSCCAL_calibration(void);
void SwitchTo8MHz(void);

#endif
