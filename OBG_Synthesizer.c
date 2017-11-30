/**
  OBG_Synthesizer.c

  This is the a source file of the AVR Butterfly
  application board adaptation of the One Bit Groovebox.

  Here are the definitions of the synthesizer
  for the One Bit Groovebox.
*/

#include <stdint.h>

#include <avr/pgmspace.h>

#include "OBG_Synthesizer.h"
#include "OBG_Sequencer.h"		/* For saving data into note grid. */
#include "OBG_Data.h"			/* Lookup tables. */
#include "ADC.h"				/* Analog input value. */
#include "Display.h"			/* Display value of the pressed button. */
#include "Menu.h"				/* Button pressed. */
#include "Speaker.h"			/* Synthesizer output. */


/** Synthesizer bit for oscillator. */
uint8_t synth_bit_osc;

/** Synthesizer bit for storing result. */
uint8_t synth_bit;

/** Synthesizer bit for temporary operations. */
uint8_t synth_bit_tmp;

/** Current waveform. */
loaded_waveform_t current_waveform;

/** Divider (creates pitches). */
uint16_t divider = 1;

/** Divider reference. */
uint16_t divider_reference;

/** Effect mode. */
effect_t effect = RING;

/* Combinational filter buffer of 64 bits. */
uint8_t comb_hist[8];
/* Index of the current bit. */
uint8_t comb_current_bit_idx;
/* Depth of the filter. */
uint8_t comb_depth;
/* Index of the back bit. */
uint8_t comb_back_bit_idx;

/* Ring modulator period. */
uint16_t ringmod_period = 0;
/* Ring modulator phase. */
uint16_t ringmod_phase = 0;
/* Ring modulator pulse width. */
uint16_t ringmod_pulsewidth = 32768;

/* Arpeggiator ticks. */
uint16_t arp_ticks = 0;
/* Arpeggiator time. */
uint16_t arp_time = 1000;
/* Arpeggiator step. */
uint8_t arp_step = 0;
/* Arpeggiator length. */
uint8_t arp_length = 3;
/* Arpeggiator divider reference. */
uint8_t arp_divider_reference;
/* Arpeggiator divider. */
uint16_t arp_divider[3] = {99, 53, 17};

/* Triangle wave direction. */
uint8_t tri_dir = 0;
/* Triangle wave phase. */
uint16_t tri_phase = 0;
/* Triangle wave delta. */
uint16_t tri_delta = 32768;

/* Noise register. */
uint16_t noise_reg = 1;
/* Noise register 2. */
uint16_t noise_reg2 = 1;
/* Noise count. */
uint16_t noise_count = 0;
/* Noise hold. */
uint16_t noise_hold = 10;
/* Noise level. */
uint8_t noise_level;
/* Noise TPDF. */
uint16_t TPDF;
/* Noise threshold. */
uint16_t noise_threshold;


/** Waveform bit. */
void Waveform_Bit(void)
{
  /* If begin of the cycle, reload waveform, else shift waveform. */
  if(current_waveform.index == 0)
  {
    current_waveform.playing = current_waveform.loaded;
  }
  else
  {
    current_waveform.playing >>= 1;
  }

  /* Set osc bit. */
  synth_bit_osc = current_waveform.playing & 0x1;

  /* Go to the next bit. */
  if(current_waveform.index < WAVEFORM_BITS-1)
  {
    current_waveform.index++;
  }
  else
  {
    current_waveform.index = 0;
  }
}

/** Update noise generator. */
void Update_Noise(void)
{
  /* 16-bit taps are 16,15,13,4 (1-based)
     15-bit taps are 15,14  -- N.V.       */

  uint16_t b1, b2, b3, b4;

  b1 = ((noise_reg & 0x8000) >> 15);
  b2 = ((noise_reg & 0x4000) >> 14);
  b3 = ((noise_reg & 0x1000) >> 12);
  b4 = ((noise_reg & 0x0008) >>  4);
  b1 = (b1 ^ b2 ^ b3 ^ b4) & 0x1;
  noise_reg <<= 1;
  noise_reg |= b1;

  b1 = ((noise_reg2 & 0x4000) >> 14);
  b2 = ((noise_reg2 & 0x2000) >> 13);
  b1 = (b1 ^ b2) & 0x1;
  noise_reg2 <<= 1;
  noise_reg2 |= b1;
}


/** Get effect mode. */
effect_t Get_Effect(void)
{
  return effect;
}

/** Set effect mode. */
void Set_Effect(uint8_t value)
{
  effect = value;
}

/** Effect combinational filter. */
void Effect_Comb_Filter(uint8_t crazy)
{
  /* Numbers of the byte containing the current bit and the bit itself. */
  uint8_t comb_hist_byte_num;
  uint8_t comb_hist_bit_num;
  
  /* Container for the byte containing the current bit. */
  uint8_t comb_hist_byte;


  /* Read back bit. */
    /* circular buffer of 64 bits -- N.V. */
  comb_back_bit_idx = (comb_current_bit_idx - comb_depth) & 0x3F;
  comb_hist_byte_num = comb_back_bit_idx >> 3;
  comb_hist_bit_num = comb_back_bit_idx & 0x7;
  comb_hist_byte = comb_hist[comb_hist_byte_num];
  synth_bit_tmp = comb_hist_byte >> comb_hist_bit_num;

  /* Perform effect. */
  synth_bit = synth_bit_osc ^ synth_bit_tmp;

  /* Write current bit. */
    /* circular buffer of 64 bits -- N.V. */
  comb_back_bit_idx = comb_current_bit_idx;  
  comb_hist_byte_num = comb_back_bit_idx >> 3;
  comb_hist_bit_num = comb_back_bit_idx & 0x7;
  comb_hist_byte = comb_hist[comb_hist_byte_num];
  comb_hist_byte = comb_hist_byte & (0xFF ^ (0x1 << comb_hist_bit_num));

  /* Perform the effect function. */
  if(! crazy)
  {
    /* should be osc, otherwise ridiculuous feedback -- N.V. */
    comb_hist_byte |= (synth_bit_osc << comb_hist_bit_num);
  }
  else
  {
    /* ridiculuous feedback -- N.V. */
    comb_hist_byte |= (synth_bit << comb_hist_bit_num);
  }

  /* Write back into history buffer. */
  comb_hist[comb_hist_byte_num] = comb_hist_byte;

  /* Advance write pointer. */
  comb_current_bit_idx = (comb_current_bit_idx + 1) & 0x3F;  
}

/** Effect ring modulator. */
void Effect_Ring_Modulator(void)
{
  /* ringmod = phase accumulator osc. -- N.V. */
 
  ringmod_phase += ringmod_period;

  if(ringmod_phase > ringmod_pulsewidth)
  {
    synth_bit = synth_bit_osc ^ 1;
  }
}

/** Effect arpeggiator. */
void Effect_Arpeggiator(void)
{
  /* arpeggiator.  This is basically clean,
     except that we change the pitch/divider every N ticks -- N.V. */


  if(arp_ticks < arp_time)
  {
    arp_ticks++;
  }
  else
  {
    arp_ticks = 0;

	if(arp_step < arp_length)
	{
	  arp_step++;
	}
	else
	{
	  arp_step = 0;
	}

	divider = arp_divider[arp_step];
  }
}

/** Effect chord. */
void Effect_Chord(void)
{
  /* chord mode.  Like arpeggiator mode and dual mode: three
     notes are played simlutaneously, from the ArpDivs.  The mixing is like
     dual mode though.  it should work much better for three notes than for two.
     osc 1 is the 1-bit waveform.  osc 2 is the ring mod.  osc 3 is the tri -- N.V. */


  synth_bit_tmp = synth_bit_osc;

  ringmod_phase += arp_divider[1] >> 1;
  if(ringmod_phase > 0x7FFF)
    synth_bit_tmp++;

  tri_phase += arp_divider[2] >> 2;
  if(tri_phase > 0x7FFF)
    synth_bit_tmp++;
    
  synth_bit_osc = (synth_bit_tmp >= 2) ? 1 : 0;
}

/** Effect tremolo. */
void Effect_Tremolo(void)
{
  /* use a triangle wave to "modulate the amplitude" -- N.V. */


  if(tri_dir == 0)
  {
    /* Modulate here the amplitude. -- W.B. */
    synth_bit_osc = 0;

    tri_phase += tri_delta;

    if(tri_phase > 32000)
    {
      tri_phase -= tri_delta + 9;
      tri_dir = 1 - tri_dir;
    }
  }
  else
  {
    tri_phase -= tri_delta;

    if(tri_phase > 32000)
    {
      tri_phase += tri_delta;
      tri_dir = 1 - tri_dir;
    }
  }
}

/** Effect dual. */
void Effect_Dual(void)
{
  /* uses the ring mod oscillator as a second voice
     mixes them by summing, adding noise, quantizing -- N.V. */


  ringmod_phase += ringmod_period;
  synth_bit_tmp = synth_bit_osc;
  if(ringmod_phase > ringmod_pulsewidth)
    synth_bit_tmp++;	/* two-bit summed oscs -- N.V. */
  synth_bit_tmp *= 16;

  if(noise_count < noise_hold)
  {
    noise_count++;
  }
  else
  {
    Update_Noise();
    noise_count = 0;
  }

  TPDF = noise_level * ((noise_reg + noise_reg2) & 0xFF);
  TPDF >>= 8;
  noise_threshold = 16 + (noise_level >> 1);
  synth_bit_osc = 0;
  if((TPDF + synth_bit_tmp) >= noise_threshold)
  {
    synth_bit_osc = 1;
  }
}


/** Set comb filter depth. */
void Set_Comb_Filter_Depth(uint8_t depth)
{
  depth ^= 0xFF;
  comb_depth = 1 + (depth >> 2);  /* +1 b/c 0 sounds like 63 -- N.V. */
}

/** Set ring modulator period. */
void Set_Ring_Modulator_Period(uint8_t period)
{
  uint16_t result;

  period ^= 0xFF;
  period >>= 2; /* should adjust formula below for speed -- N.V. */

  result = 8 * LOAD_FREQUENCY_DIVIDER(period);

  if(period == 63)
  {
    result = 0;
  }

  ringmod_period = result;
}

/** Set ring modulator pulse width. */
void Set_Ring_Modulator_Pulse_Width(uint8_t pulse_width)
{
  /* input : 0x00 - 0xff
     output: 16-bit val range from 0x0000 to 0x7fff
     0 0 0 0   0 0 0 0   0 0 0 0   0 0 0 0  (0x0000)
     0 1 1 1   1 1 1 1   1 1 1 1   1 1 1 1  (0x7fff)
     x 7 6 5   4 3 2 1   0 x x x   x x x x  (tmp<<7)
     x x x x   x x x x   x 7 6 5   4 3 2 1  (tmp>>1) */

  uint16_t tmp = pulse_width;

  ringmod_pulsewidth = (tmp << 7) | ( tmp >> 1);
}

/** Set noise level. */
void Set_Noise_Hold(uint8_t level)
{
  noise_hold = pgm_read_word(&Noise_Hold_Table[(level >> 2) & 0x7]);
}

/** Set noise level. */
void Set_Noise_Level(uint8_t level)
{
  noise_level = level;
}

/** Set arpeggiator dividers. */
void Set_Arpeggiator_Dividers(uint8_t level)
{
  uint8_t value1;
  uint8_t value2;
  uint8_t value3;

  arp_divider_reference = level;

  level >>= 1;

  arp_divider[0] = LOAD_FREQUENCY_DIVIDER(divider_reference);

  value1 = pgm_read_byte(&Interval_Vs[level]);

  value2 = divider_reference + (value1 >> 4);

  /* while loops are scary..  mod is too slow -- N.V. */
  if(value2 > 63) value2 -= 12;
  if(value2 > 63) value2 -= 12;  

  arp_divider[1] = LOAD_FREQUENCY_DIVIDER(value2);
  
  value3 = value1 & 0xF;

  if(value3 == 0)
  {
    arp_length = 2;
  }
  else
  {
    arp_length = 3;

	value3 += value2;

    if(value3 > 63) value3 -= 12;
    if(value3 > 63) value3 -= 12;  

    arp_divider[2] = LOAD_FREQUENCY_DIVIDER(value3);
  }
}

/** Set arpeggiator time. */
void Set_Arpeggiator_Time(uint8_t time)
{
  /* small, accurate exponential conversion
     input : 0-255. 
     output: 1+2^(t1/16), nominal range: 1 <=> 65535, actual range: 1 <=> 0xf501(62721)
     note: arpTime should be non-zero */
  uint16_t op;
  time = 0xFF - time;
  op = pgm_read_byte(&Power_Reference[time & 0xF]) << 8;
  arp_time = 1 + (op >> (15 - (time >> 4)));
}

/** Set triangle wave period. */
void Set_Triangle_Wave_Period(uint8_t period)
{
  /* LFO used for Tremelo, very sim. to arpeggiator -- N.V. */
  uint16_t op;
  op = pgm_read_byte(&Power_Reference[period & 0xF]) << 8;
  tri_delta = (op >> (15 - (period >> 4))) >> 2;
}


/** Set current waveform. */
void Set_Current_Waveform(uint8_t waveform_index)
{
  /* Load current waveform and initialize bit index. */
  current_waveform.loaded = pgm_read_word(&Waveforms[waveform_index]);
  current_waveform.index = 0;
}

/** Set current divider. */
void Set_Current_Divider(uint8_t div)
{
  /* Used for arpeggiator. */
  divider_reference = div >> 2;

  divider = LOAD_FREQUENCY_DIVIDER(div);
}

/** Set current effect parameter 1. */
void Set_Current_EffParam1(uint8_t effparam1)
{
  switch(effect)
  {
    case COMB:
	case COMBCRAZY:
	  Set_Comb_Filter_Depth(effparam1);
	  break;

    case RING:
	case DUAL:
	  Set_Ring_Modulator_Period(effparam1);
	  break;

    case TREM:
	  Set_Triangle_Wave_Period(effparam1);
	  break;

    case ARP:
	case CHORD:
	  Set_Arpeggiator_Dividers(effparam1);
	  break;

    default:
	  break;
  }
}

/** Set current effect parameter 2. */
void Set_Current_EffParam2(uint8_t effparam2)
{
  switch(effect)
  {
    case RING:
	  Set_Ring_Modulator_Pulse_Width(effparam2);
	  break;

    case DUAL:
	  Set_Ring_Modulator_Pulse_Width(0x7F);
	  Set_Noise_Hold(effparam2);
	  Set_Noise_Level(effparam2);
	  break;

    case ARP:
	  Set_Arpeggiator_Time(effparam2);
	  break;

    default:
	  break;
  }
}

/** Effect bit. */
void Effect_Bit(void)
{
  switch(effect)
  {
    case DRY:
	  goto DO_INDIRECT_EFFECT;

    case COMB:
	  Effect_Comb_Filter(0);
	  break;

    case RING:
	  Effect_Ring_Modulator();
	  break;

    case COMBCRAZY:
	  Effect_Comb_Filter(1);
	  break;

    case ARP:
	  Effect_Arpeggiator();
	  goto DO_INDIRECT_EFFECT;

    case CHORD:
	  Effect_Chord();
	  goto DO_INDIRECT_EFFECT;

    case TREM:
	  Effect_Tremolo();
	  goto DO_INDIRECT_EFFECT;

    case DUAL:
	  Effect_Dual();
	  goto DO_INDIRECT_EFFECT;

    default:
	  break;

DO_INDIRECT_EFFECT:
      synth_bit = synth_bit_osc;
	  break;
  }
}

/** OBG keys processing. */
void OBG_Keys(void)
{
  uint8_t obg_input = ADC_Result();
  synth_button_t obg_button = Get_Button();

  switch(obg_button)
  {
    case BUTTON_WAVEFORM_RECORD:
	  Save_Current_Note_Parameter(WAVEFORM, obg_input >> 2);
    case BUTTON_WAVEFORM_JAM:
	  Set_Current_Waveform(obg_input >> 2);
	  goto DISPLAY_VALUE_OF_THE_BUTTON;

    case BUTTON_PITCH_RECORD:
	  Save_Current_Note_Parameter(DIVIDER, obg_input >> 2);
	case BUTTON_PITCH_JAM:
	  Set_Current_Divider(obg_input >> 2);
	  if(effect == ARP || effect == CHORD)
	  {
	    Set_Arpeggiator_Dividers(divider_reference);
      }
	  goto DISPLAY_VALUE_OF_THE_BUTTON;

    case BUTTON_EFFPARAM1_RECORD:
	  Save_Current_Note_Parameter(EFFPARAM1, obg_input);
    case BUTTON_EFFPARAM1_JAM:
	  Set_Current_EffParam1(obg_input);
	  goto DISPLAY_VALUE_OF_THE_BUTTON;

    case BUTTON_EFFPARAM2_RECORD:
	  Save_Current_Note_Parameter(EFFPARAM2, obg_input);
    case BUTTON_EFFPARAM2_JAM:
	  Set_Current_EffParam2(obg_input);
	  goto DISPLAY_VALUE_OF_THE_BUTTON;

    case BUTTON_EFFECT_MODE:
	  Set_Effect(obg_input >> 5);

DISPLAY_VALUE_OF_THE_BUTTON:
      /* Show value of button. */
	  Display_Value(obg_button, obg_input);
	  break;

    default:
	  break;
  }
}

/** OBG synthesizer initialization. */
void OBG_Synthesizer_Init(void)
{
  /* Initialize waveform. */
  Set_Current_Waveform(0);

  /* Initialize pitch. */
  Set_Current_Divider(0);
}

/** OBG synthesizer task. */
void OBG_Synthesizer_Task(void)
{
  static uint16_t divider_phase = 0;

  /* Create pitch. */
  if(divider_phase < divider)
  {
    divider_phase++;
  }
  else
  {
    divider_phase = 0;

    /* Generate new sound bit. */
    Waveform_Bit();

	/* Run effect on it. */
	Effect_Bit();

	/* Write it to speaker. */
//	Speaker_Output(synth_bit_osc);
	Speaker_Output(synth_bit);
  }
}
