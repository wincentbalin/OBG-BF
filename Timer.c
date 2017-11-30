/**
  Clock.c

  This is the a source file of the AVR Butterfly
  application board adaptation of the One Bit Groovebox.

  Here go the functions which work with timers.
*/


#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "Timer.h"
#include "OBG-BF.h"
#include "OBG_Synthesizer.h"	/* To start synthesizer task immediately. */
#include "OBG_Sequencer.h"		/* To start sequencer task immediately. */
#include "OBG_Tempo.h"			/* To set tap timestamp immediately. */


/** Timer2 value. */
uint8_t timer2_value = 0;

/** Timer2 overflow counter. */
uint16_t timer2_overflow = 0;

/** Initialization of timers. */
void Timer_Init(void)
{
  /* Timer 0: Synthesizer interrupt
     CTC mode, prescaler / 8 */
  TCCR0A = 1 << WGM01 | 0 << WGM00 |
           0 << CS02 | 1 << CS01 | 0 << CS00;
  OCR0A = 180; /* Fs = (8 MHz / 8) / 45 = 22222.2_ Hz */
  TIFR0 = 1 << OCF0A; 		/* Enable timer. */
  TIMSK0 |= (1 << OCIE0A);	/* Start timer. */

  /* Initialize timer 1 to do interrupts... well, when needed.
     Timer 1 will generate sequencer events. */
  TCCR1A = 0;	/* No PWM operation. */
  /* CTC mode, prescaler of 64 */
  TCCR1B = 0 << WGM13 | 1 << WGM12 | 0 << CS12 | 1 << CS11 | 1 << CS10;
  TIFR1 = 1 << OCF1A;		/* Enable timer. */
  TIMSK1 |= (1 << OCIE1A);	/* Start timer. */


  /* Timer 2 remains asynchronous, initialized when doing
     OSCCAL calibration.
	 Timer 2 will measure time between tempo taps. */
}

/** Timer 0 ISR -- set appropriate flag. */
ISR(TIMER0_COMP_vect)
{
  OBG_Synthesizer_Task();
}

/** Timer 1 ISR -- set appropriate flag. */
ISR(TIMER1_COMPA_vect)
{
  OBG_Sequencer_Task();
}

/** Set Timer 1 value. */
void Set_Timer1_Value(uint16_t value)
{
  TCNT1 = value;
}

/** Set Timer 1 output compare value. */
void Set_Timer1_OC_Value(uint16_t value)
{
  OCR1A = value;
}

/** Timer 2 trigger. */
void Timer2_Trigger(void)
{
  obg_event_tap = 1;
  /* Set timestamp of the event. */
  Set_Tap_Timestamp(Get_Timestamp());
}

/** Update Timer 2 value. */
void Update_Timer2_Value(void)
{
  uint8_t new_value;

  /* If no current update of the Timer 2, get new value. */
  if(! (ASSR & (1 << TCN2UB)))
  {
    new_value = TCNT2;

    /* If there was an overflow, increment overflow counter. */
    if(new_value < timer2_value)
    {
      timer2_overflow++;
    }

    /* Store new Timer 2 value. */
    timer2_value = new_value;
  }
}

/** Get timestamp from Timer 2 overflow counter and Timer 2 value. */
uint32_t Get_Timestamp(void)
{
  return ((uint32_t) timer2_overflow << 8) | timer2_value;
}

/** Calculate timestamp interval. */
uint32_t Calculate_Timestamp_Interval(uint32_t timestamp1, uint32_t timestamp2)
{
  /* Calculate interval value and return it. */
  return (timestamp1 > timestamp2) ?
            (0xFFFFFFFF - timestamp1) + timestamp2 :
            timestamp2 - timestamp1;
}

/** Calculate current timestamp interval. */
uint32_t Calculate_Current_Timestamp_Interval(uint32_t previous_timestamp)
{
  /* Get current timestamp. */
  uint32_t current_timestamp = Get_Timestamp();

  /* Calculate interval value and return it. */
  return Calculate_Timestamp_Interval(previous_timestamp, current_timestamp);
}

/** Timer task -- executes periodically,
    stores and calculates time using Timer 2. */
void Timer_Task(void)
{
  /* Update values from Timer 2. */
  Update_Timer2_Value();
}
