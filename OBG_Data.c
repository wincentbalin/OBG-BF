/**
  OBG_Data.c

  This is the a source file of the AVR Butterfly
  application board adaptation of the One Bit Groovebox.

  Here are the definitions of the lookup tables
  for the One Bit Groovebox.
*/

#include "OBG_Data.h"


/** Array of 1-bit waveforms. */
uint16_t Waveforms[64] PROGMEM =
{
  0x000F, 0x001F, 0x0007, 0x0003,
  0x0033, 0x004F, 0x001B, 0x0077,
  0x0067, 0x0037, 0x0027, 0x0063,
  0x00C7, 0x0047, 0x002F, 0x0017,
  0x0013, 0x00DB, 0x0043, 0x0023,
  0x000B, 0x005B, 0x0133, 0x004B,
  0x009D, 0x009B, 0x006B, 0x0057,
  0x0001, 0x0097, 0x00B3, 0x004D,
  0x00A7, 0x00CB, 0x0093, 0x008B,
  0x002B, 0x0009, 0x0011, 0x0099,
  0x005D, 0x0053, 0x0049, 0x002D,
  0x0005, 0x012D, 0x00A3, 0x0025,
  0x0021, 0x0015, 0x0045, 0x00AB,
  0x00AD, 0x014B, 0x0055, 0x012B,
  0x0089, 0x0095, 0x0125, 0x0153,
  0x00A5, 0x0155, 0x0155, 0x0155
};

/** Frequency division table.

    These are specific to 44.4444444 KHz/ 11 bit waveforms. -- N.V.
    Modified this to include "silence" at the beginning. -- N.V.
*/
uint16_t Frequency_Divider[64] PROGMEM =
{
  0x7FFF, 0x00F7, 0x00E9, 0x00DC,
  0x00D0, 0x00C4, 0x00B9, 0x00AF,
  0x00A5, 0x009C, 0x0093, 0x008B,
  0x0083, 0x007C, 0x0075, 0x006E,
  0x0068, 0x0062, 0x005D, 0x0057,
  0x0052, 0x004E, 0x0049, 0x0045,
  0x0041, 0x003E, 0x003A, 0x0037,
  0x0034, 0x0031, 0x002E, 0x002C,
  0x0029, 0x0027, 0x0025, 0x0023,
  0x0021, 0x001F, 0x001D, 0x001C,
  0x001A, 0x0019, 0x0017, 0x0016,
  0x0015, 0x0013, 0x0012, 0x0011,
  0x0010, 0x000F, 0x000F, 0x000E,
  0x000D, 0x000C, 0x000C, 0x000B,
  0x000A, 0x000A, 0x0009, 0x0008,
  0x0007, 0x0006, 0x0005, 0x0003
};

/** Table for arpeggiator. */
uint8_t Interval_Vs[128] PROGMEM =
{
  0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80,
  0x90, 0xA0, 0xB0, 0xC0, 0xD0, 0xE0, 0xF0, 0xC4,
  0xC3, 0x97, 0x75, 0x79, 0x77, 0xB5, 0x87, 0x57,
  0xA5, 0xA6, 0x78, 0x88, 0xD3, 0x4C, 0xE2, 0xC2,
  0x96, 0x84, 0x93, 0xB4, 0x6A, 0xA4, 0x66, 0x69,
  0x5A, 0x86, 0x5B, 0x59, 0x76, 0xB3, 0x85, 0xA2,
  0x3C, 0x68, 0x48, 0x94, 0x67, 0x74, 0xA3, 0xF1,
  0x58, 0x39, 0x55, 0x73, 0x4A, 0x4B, 0x65, 0x3D,
  0x47, 0x49, 0x54, 0x83, 0x64, 0x2C, 0xC1, 0xB2,
  0x45, 0x2E, 0x72, 0x92, 0x2A, 0x63, 0x37, 0xB1,
  0x46, 0x3B, 0x82, 0x3A, 0x53, 0x44, 0x2D, 0x1F,
  0x38, 0x36, 0x43, 0x52, 0x35, 0x27, 0x34, 0x1C,
  0x1E, 0x62, 0x1B, 0x25, 0xA1, 0x29, 0x28, 0x2B,
  0x71, 0x81, 0x1D, 0x26, 0x61, 0x17, 0x19, 0x33,
  0x18, 0x1A, 0x42, 0x16, 0x24, 0x32, 0x15, 0x23,
  0x41, 0x14, 0x22, 0x13, 0x31, 0x12, 0x21, 0x11
};

/** Table of powers to use with exponential function. */
uint8_t Power_Reference[16] PROGMEM =
{
  0x80,0x85,0x8B,0x91, 
  0x98,0x9E,0xA5,0xAD, 
  0xB5,0xBD,0xC5,0xCE, 
  0xE7,0xE0,0xEA,0xF5
};

/** Noise hold lookup table. */
uint16_t Noise_Hold_Table[8] PROGMEM =
{
  0x0001, 0x0002, 0x0003, 0x0004,
  0x0007, 0x0013, 0x003D, 0x04D2
};