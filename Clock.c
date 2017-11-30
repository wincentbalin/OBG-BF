/**
  Clock.c

  This is the a source file of the AVR Butterfly
  application board adaptation of the One Bit Groovebox.

  Here go the functions which manage clock frequency and time.
*/

#include <avr/io.h>
#include <avr/interrupt.h>

#include "Clock.h"

/**
  Delay for the specified amount of milliseconds.

  WARNING!
  Works only at the beginning, when clock frequency is at 1 MHz!
  After switching to 8 MHz use 8*millisec !

  Taken over form the original AVR Butterfly firmware.
*/
void Delay(unsigned int millisec)
{
	unsigned char i;
    
    while (millisec--)
			for (i=0; i<125; i++)  
				asm volatile ("nop"::);
}

/**
  Calibrate built-in clock generator using 32.768 KHz crystal.

  Taken over form the original AVR Butterfly firmware.
*/
void OSCCAL_calibration(void)
{
    unsigned char calibrate = 0;
    int temp;
    unsigned char tempL;

    CLKPR = (1<<CLKPCE);        // set Clock Prescaler Change Enable
    // set prescaler = 8, Inter RC 8Mhz / 8 = 1Mhz
    CLKPR = (1<<CLKPS1) | (1<<CLKPS0);
    
    TIMSK2 = 0;             //disable OCIE2A and TOIE2

    ASSR = (1<<AS2);        //select asynchronous operation of timer2 (32,768kHz)
    
    OCR2A = 200;            // set timer2 compare value 

    TIMSK0 = 0;             // delete any interrupt sources
        
    TCCR1B = (1<<CS10);     // start timer1 with no prescaling
    TCCR2A = (1<<CS20);     // start timer2 with no prescaling

    while((ASSR & 0x01) | (ASSR & 0x04));       //wait for TCN2UB and TCR2UB to be cleared

    Delay(1000);    // wait for external crystal to stabilise
    
    while(!calibrate)
    {
        cli(); // mt __disable_interrupt();  // disable global interrupt
        
        TIFR1 = 0xFF;   // delete TIFR1 flags
        TIFR2 = 0xFF;   // delete TIFR2 flags
        
        TCNT1H = 0;     // clear timer1 counter
        TCNT1L = 0;
        TCNT2 = 0;      // clear timer2 counter
           
        // shc/mt while ( !(TIFR2 && (1<<OCF2A)) );   // wait for timer2 compareflag    
        while ( !(TIFR2 & (1<<OCF2A)) );   // wait for timer2 compareflag

        TCCR1B = 0; // stop timer1

        sei(); // __enable_interrupt();  // enable global interrupt
    
        // shc/mt if ( (TIFR1 && (1<<TOV1)) )
        if ( (TIFR1 & (1<<TOV1)) )
        {
            temp = 0xFFFF;      // if timer1 overflows, set the temp to 0xFFFF
        }
        else
        {   // read out the timer1 counter value
            tempL = TCNT1L;
            temp = TCNT1H;
            temp = (temp << 8);
            temp |= tempL;
        }
    
        if (temp > 6250)
        {
            OSCCAL--;   // the internRC oscillator runs to fast, decrease the OSCCAL
        }
        else if (temp < 6120)
        {
            OSCCAL++;   // the internRC oscillator runs to slow, increase the OSCCAL
        }
        else
            calibrate = 1;   // the interRC is correct

        TCCR1B = (1<<CS10); // start timer1
    }
}

/**
  Switch to 8 MHz.

  Simply disable prescaler.
*/
void SwitchTo8MHz(void)
{
  /* Enable prescaler change. */
  CLKPR = (1 << CLKPCE);
  /* Clear prescaler. */
  CLKPR = 0;
}
