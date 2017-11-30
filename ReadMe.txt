One Bit Groovebox for AVR Butterfly
-----------------------------------

___________________________________
ATTENTION:

You will need hardware additions
to the original AVR hardware
to use this design!
___________________________________



Original design and code by Noah Vawter,
nvawter (at) media (dot) mit (dot) edu,
port to AVR Butterfly by Wincent Balin,
wincent (dot) balin (at) gmail (dot) com.

AVR Butterfly specific code in files
LCD_Drive.[ch], LCD_Functions.[ch]
was created by ATMEL Norway and ported
to AVR GCC compiler by Martin Thomas et al.
Most of the platform-specific functions
were derived from the port of the original
firmware to AVR GCC compiler.

The license of the original software is
unknown. For purposes of licensing refer
to Noah Vawter directly. Should the original
code become (re-)licensed, the port goes
automatically under the same license.

Programming was done in AVR Studio,
using AVR GCC compiler. The project file
is OBG-BG.aps. The names of the source
files should be quite self-explaining.

The descriptions of buttons of the original
hardware are printed on display now. Use
the described function pressing joystick;
change the value of function utilizing
the potentiometer added like in the figure
below (use monospaced font like Courier New
to display the figure correctly):

     +-------- V+
     |
     |
     |
    +++
  r | | <----- 100k
   \| |
    \ |
    |\|
    | \
    | |\______ Voltage Reader + (1)
    | |
    | |
    +++
     |
     |
     |
     +-------- Voltage Reader - (2)


Change to another function using joystick.
The order of functions is displayed below:

+---------------+------------------+
| WAVEFORM_JAM  | WAVEFORM_RECORD  |
| PITCH_JAM     | PITCH_RECORD     |
| EFFPARAM1_JAM | EFFPARAM1_RECORD |
| EFFPARAM2_JAM | EFFPARAM2_RECORD |
| EFFECT_MODE   | TAP_TEMPO        |
+---------------+------------------+


Remember that in the lowest position
of the waveform's frequency the output
is just one click in a couple of seconds.
This is done to make erasing of sound
possible.

The effects utilized are the same
as in the original device; look them up
in the source code.

To set the tempo of the sequencer,
go to the function "Tap tempo" and tap
the quarter notes utilizing joystick.

The output of the groovebox goes directly
to the buzzer. Also, one can get the output
from the AVR Butterfly pins PB5 (signal)
and PB3 (ground). Beware of the relatively
high output voltage of 3V though! Voltage
divider is the way to go here!

The home page of the original code is
http://web.media.mit.edu/~nvawter/projects/1bit/

The page of the discussion group is
http://launch.groups.yahoo.com/group/onebitgroove/

The launchpad page is https://launchpad.net/bitbox
