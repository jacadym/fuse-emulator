/* spectrum.c: Generic Spectrum routines
   Copyright (c) 1999-2004 Philip Kendall, Darren Salt

   $Id$

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

   Author contact information:

   E-mail: pak21-fuse@srcf.ucam.org
   Postal address: 15 Crescent Road, Wokingham, Berks, RG40 2DB, England

*/

#include <config.h>

#include <libspectrum.h>

#include "compat.h"
#include "display.h"
#include "event.h"
#include "keyboard.h"
#include "machine.h"
#include "memory.h"
#include "printer.h"
#include "rzx.h"
#include "settings.h"
#include "sound.h"
#include "sound/lowlevel.h"
#include "spectrum.h"
#include "tape.h"
#include "timer.h"
#include "z80/z80.h"

/* 272Kb of RAM */
libspectrum_byte RAM[ SPECTRUM_RAM_PAGES ][0x4000];

/* How many tstates have elapsed since the last interrupt? (or more
   precisely, since the ULA last pulled the /INT line to the Z80 low) */
libspectrum_dword tstates;

/* The last byte written to the ULA */
libspectrum_byte spectrum_last_ula;

/* Set these every time we change machine to avoid having to do a
   structure lookup too often */
spectrum_contention_delay_function contend_delay;

libspectrum_byte spectrum_contention[ 80000 ];

int
spectrum_frame( void )
{
  libspectrum_dword frame_length;
  int error;

  /* Reduce the t-state count of both the processor and all the events
     scheduled to occur. Done slightly differently if RZX playback is
     occuring */
  frame_length = rzx_playback ? tstates
			      : machine_current->timings.tstates_per_frame;

  error = event_frame( frame_length ); if( error ) return error;
  tstates -= frame_length;
  if( z80.interrupts_enabled_at >= 0 )
    z80.interrupts_enabled_at -= frame_length;

#ifdef ALWAYS_USE_TIMER /* Some sound routines do not provide speed control */
  if( sound_enabled ) sound_frame();
  timer_sleep();
#else			/* #ifdef ALWAYS_USE_TIMER */
  if(sound_enabled) {
    sound_frame();
  } else {
    timer_sleep();
  }
#endif			/* #ifdef ALWAYS_USE_TIMER */

  if(display_frame()) return 1;
  printer_frame();

  /* Add an interrupt unless they're being generated by .rzx playback */
  if( !rzx_playback ) {
    if( event_add( machine_current->timings.tstates_per_frame,
		   EVENT_TYPE_FRAME ) ) return 1;
  }

  return 0;
}

/* What happens if we read from an unattached port? */
libspectrum_byte
spectrum_unattached_port( int offset )
{
  int line, tstates_through_line, column;

  /* Return 0xff (idle bus) if we're in the top border */
  if( tstates < machine_current->line_times[ DISPLAY_BORDER_HEIGHT ] )
    return 0xff;

  /* Work out which line we're on, relative to the top of the screen */
  line = ( (libspectrum_signed_dword)tstates -
	   machine_current->line_times[ DISPLAY_BORDER_HEIGHT ] ) /
    machine_current->timings.tstates_per_line;

  /* Idle bus if we're in the lower or upper borders */
  if( line >= DISPLAY_HEIGHT ) return 0xff;

  /* Work out where we are in this line */
  tstates_through_line = tstates -
    machine_current->line_times[ DISPLAY_BORDER_HEIGHT + line ];

  /* Idle bus if we're in the left border */
  if( tstates_through_line < machine_current->timings.left_border - offset )
    return 0xff;

  /* Or the right border or retrace */
  if( tstates_through_line >= machine_current->timings.left_border +
                              machine_current->timings.horizontal_screen -
                              offset )
    return 0xff;

  column = ( ( tstates_through_line + 1 -
	       machine_current->timings.left_border ) / 8 ) * 2;

  switch( tstates_through_line % 8 ) {

    /* The pattern of bytes returned here is the same as documented by
       Ramsoft in their 'Floating bus technical guide' at
       http://www.ramsoft.bbk.org/floatingbus.html

       However, the timings used are based on the first byte being
       returned at 14335 (48K) and 14361 (128K) respectively, not
       14347 and 14368 as used by Ramsoft.

       In contrast to previous versions of this code, Arkanoid and
       Sidewize now work. */

    /* Attribute bytes */
    case 3: column++;
    case 1:
      return RAM[ memory_current_screen ][ display_attr_start[line] + column ];

    /* Screen data */
    case 2: column++;
    case 0:
      return RAM[ memory_current_screen ][ display_line_start[line] + column ];

    /* Idle bus */
    case 4: case 5: case 6: case 7:
      return 0xff;

  }

  return 0;		/* Keep gcc happy */
}
