/**
  Timer.h

  This is the a header file of the AVR Butterfly
  application board adaptation of the One Bit Groovebox.
*/

#ifndef OBG_BF_TIMER_H
#define OBG_BF_TIMER_H

#include <stdint.h>

/* Prototypes of functions. */
void Timer_Init(void);
void Timer_Task(void);
void Set_Timer1_Value(uint16_t value);
void Set_Timer1_OC_Value(uint16_t);
//void Set_Timer1_Adjustment(uint16_t);
void Timer2_Trigger(void);
uint32_t Get_Timestamp(void);
uint32_t Calculate_Timestamp_Interval(uint32_t, uint32_t);
uint32_t Calculate_Current_Timestamp_Interval(uint32_t);

#endif
