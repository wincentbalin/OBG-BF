/**
  OBG-BF.c

  This is the main source file of the AVR Butterfly
  application board adaptation of the One Bit Groovebox.

  This file provides the means to start the application.
*/

#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "OBG-BF.h"
#include "Clock.h"
#include "Timer.h"
//#include "Uart.h"
#include "Menu.h"
#include "ADC.h"
#include "Display.h"
#include "Speaker.h"
#include "OBG_Synthesizer.h"
#include "OBG_Sequencer.h"
#include "OBG_Tempo.h"

/** Tap event. */
uint8_t obg_event_tap = 0;

/**
  Calibration of frequency.
*/
void Calibration(void)
{
  /* Calibrate clock at 1 MHz using 32.768 KHz crystal. */
  OSCCAL_calibration();
}

/**
  Intialization of peripherals.
*/
void Initialization(void)
{
  /* Switch to 8 MHz. */
  SwitchTo8MHz();

  /* Initialize timers. */
  Timer_Init();

  /* Initialize UART. 115200 baud, 8N1. Doubled oversampling. */
//  Uart_Init(8);

  /* Initialize (LCD) display. */
  Display_Init();

  /* Initialize ADC. */
  ADC_Init();

  /* Initialize menu. */
  Menu_Init();

  /* Initialize speaker. */
  Speaker_Init();
}

/** Initialization of One Bit Groovebox. */
void OBG_Init(void)
{
  /* Initialize synthesizer. */
  OBG_Synthesizer_Init();

  /* Initialize sequencer. */
  OBG_Sequencer_Init();
}

/**
  Application entry point.
*/
int main(void)
{
  /* Calibrate frequency. */
  Calibration();

  /* Initialize peripherals. */
  Initialization();

  /* Initialize One Bit Groovebox. */
  OBG_Init();

  /* After initializing everything, enable interrupts. */
  sei();


  /* Main loop.
  
     The flags are set through interrupts asynchronously.
	 If a flag is set, the corresponding task gets executed
	 and the flag cleared.
	 Then auxilary functions (key processing, ADC value calculation)
	 get executed.
	 At the end sleeping is recommended.
  */
  while(1)
  {
    /* Tap event. */
	if(obg_event_tap == 1)
	{
	  obg_event_tap = 0;
	  OBG_Tempo_Task();
	}

	/* Timer task. */
	Timer_Task();

	/* Menu task. */
	Menu_Task();

	/* Display task. */
	Display_Task();

	/* UART task. */
//	Uart_Task();
  }

  return 0;
}
