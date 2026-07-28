This directory folder has code for a liquid crystal display.

lcd_<display panel part no.>_<meter equation> is a big switch statement
called when new data arrives. It calls AFE routines to get new data.
It calls LCD display routines to put the new data on the panel.

LCD_<display panel part no.> has the tables to drive a display panel.
e.g. LCD_Bq14053.c has the tables to drive the BQ14053 LCD panel on
the demo PCB. (There is much more about this below.)

lcd_strings_en.c are English strings used in the LCD interface.
Other languages and strings are possible.

----

To remove the LCD from the code, use the driver stub lcd_stub.c,
which replaces everything. Later, to clean up the code, one
can leave it out, and use the link errors to find and remove the calls.

----

I apologize about the length of the discussion that follows. However,
when a person needs to code a new LCD panel, the discussion is too short!
When reading it, really look at the code.

To modify the LCD for a new panel, first understand the scheme:
lcd.c breaks a number or ASCII string down to individual segments of the
LCD panel. Then it lights the segments.
A segment is lit when a "common" bit, is turned to one in a "pin"s LCD register.
The common and pin signal intersect in a display panel, and light the segment.

So, see how lcd.c writes a number to the LCD.
To break data down, lcd.c first makes a character. Then it steps through a 
table of character positions.
Each character position points to a table of segment entries.
Each segment entry has a bit mask of the "common" and a pin number of the
register to write the bit mask.

If you read lcd.c, the "segment write" logically-ors the bit mask to
the register. Why?
After a lot of experimentation, the firmware group here found that a
good way to manage an LCD panel is to treat it like paper and pencil.
First it is cleared, and then marks are added to it.
In that way, there are no "left over" marks forgotten from earlier
writes or special modes. The clear and write is so fast that it appears
instant. Even when the CPU is very slow, the writes are in a logical
sequence that looks reasonable.

The character table (e.g. in LCD_BQ14053.c) is how lcd.c can find
a character position in the LCD panel. The character table has 
the address of a table for each digit, e.g. digit 1, 2, 3, etc. 
The digit tables then have an entry for every segment in the
digit. E.g. digit 3's fifth segment, E will be a symbol
"_3E". _3E will be #defined as something like (COM0|PIN01)
(e.g. in LCD_BQ14053.h)

It is unpleasant to rewrite a lot of LCD code when a PCB has a small reroute.
So, COM0 and PIN01 are unchanging -names- for pins on the -LCD panel-.
These -names- don't -change- on -different- PCBs, because the -panel- is same.

The -numbers- change, because PIN01 can be attached to any LCD pin on the IC.
Likewise, the -name- COM0 can can have the pin number of any common.
So, PIN01 and COM0 are #defined in a "PCB file" (e.g. mapeb11_1.h)
just before that PCB file includes the definition of the LCD panel
(e.g. LCD_BQ14053.h)
In that way, rerouting LCD pins is just rewriting
a few numbers in the PCB file's #defines. If the PCB is completely redesigned,
just rewrite the numbers in a different PCB file for the new PCB.
In this way, the LCD panel's tables, fonts, code and lcd.c all remain the same.

By convention, index 0, the first position of the character table, is unused.
The second entry (index 1) in a character table is character 1. 
1 is the first (upper-left) character. So number 1 is the first character,
written left-to-right, top-to-bottom. For other languages, this
direction can be changed by changing the tables.

By convention, the first segment of a character, e.g. segment A,
is the first segment of a table of segments for a character position.
The last segment is a -decimal point-, if it is physically present.

There is an auxiliary table telling how many segments are in each
character position table. So, lcd.c does not run off the edge of the
character tables. This means that fonts can have too many segments,
and decimal points can be after the last possible segment of a font.
An attempt to write a segment beyond the digit's table will just fail
silently.

The number of characters and the starting character of each line
are #defines in the panel's .h file (e.g. see LCD_BQ14053.h)
The high-level code, the switch that writes different displays,
can write starting at the lines, for the given number of characters.

There is a special array of "icon" segments that can be written individually.
These are for graphic icons, but it is also convenient to put all the
decimal points in the first part of the table. In that way, they
can be used for special number formats, periods or even graphics.
Decimal points should be defined even if they are not present, so
that code that uses them has no special cases.

Non-existent segments can just have a pin number that is outside the
permitted range of lcd.c So, define a null segment, and use it.

By convention, the icon array is the last "character" in the character table.
In this way, all the -valid- segments can be written, cleared, examined, 
etc. by stepping a nested loop through all the characters in the character
table.

lcd.c uses this nested loop trick to find and configure all the commons
and pins used by an LCD panel. (This is most of the code in lcd_init())
After this is done the first time, by a programmer with a debugger in 
the engineering lap, the set-up register values can be read in a debugger, 
and then added to the PCB's .h file. This cuts out most code in lcd_init().
And, the LCD starts up very fast.

lcd.c uses a "font" (e.g. lcd_font_7.h) to find which segments to turn on.
It puts numebrs from the "font" into a font table.
In these fonts, bit 0 is segment A, bit 1 is segment B, etc.
This font (e.g. lcd_font_14_7.h) is included as part of the definition file 
(e.g. LCD_BQ14053.h) for the LCD panel.
The choice of font is always the same for an LCD panel.

LCD_BQ14053.h includes a combination 14-segment and 7=segment font,
(lcd_font_14_7.h) In this way, the font can be reused for different panels.
This dual font (lcd_font_14_7.h) has the same segments for segments A..G 
(the 7-segment part), and adds more for the 14-segment characters. So, the 
7-segment character positions stop writing segments at the end of the 7-segment 
digit tables. The 14-segment digit tables are longer, so more segments write.

The LCD description tables (e.g. LCD_BQ14053.c) use the definitions
in the LCD include file (LCD_BQ14053.h) For example _3E, segment "E"
of numeral 3, is defined as (COM0|PIN01)  COM0 is a bit-mask.
lcd.c, the driver, logically-ors COM0 from this number to the register number 
defined as PIN01.

_3E is placed in position E of digit 3's digit table.
In turn, the address of digit-3's table is placed in the fourth entry,
index 3, of the character table.
One issue is that some 14-segment panels number the last segments in different
sequence. This can be fixed without changing the font by writing the segments
in a different order in the digit table.

The PCB file mapeb11_1.h can and should be a different name 
for a different PCB. This file is included in options.h, and can be 
changed easily for each build.
In demo codes, "EB" stands for "E"valuation "B"oard for the m"11",
version "1".

$Id: _readme.txt 25648 2020-06-10 11:00:29Z phuddar $

