/* debugger.c: the debugger
   Copyright (c) 2002-2003 Philip Kendall

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

#ifdef USE_WIDGET

#include <stdio.h>

#include <libspectrum.h>

#include "debugger/debugger.h"
#include "widget_internals.h"
#include "z80/z80.h"
#include "z80/z80_macros.h"

int
ui_debugger_activate( void )
{
  return widget_do( WIDGET_TYPE_DEBUGGER, NULL );
}

int
ui_debugger_deactivate( int interruptable )
{
  return 0;
}

int
widget_debugger_draw( void *data GCC_UNUSED )
{
  const char *register_names[] = { "PC", "SP",
				   "AF", "AF'",
				   "BC", "BC'",
				   "DE", "DE'",
				   "HL", "HL'",
                                 };
  libspectrum_word *register_values[] = { &PC, &SP,
					  &AF, &AF_,
					  &BC, &BC_,
					  &DE, &DE_,
					  &HL, &HL_,
					};

  size_t i; char buffer[40];

  /* Blank the main display area */
  widget_dialog_with_border( 1, 2, 30, 11 );

  widget_printstring( 11, 2, WIDGET_COLOUR_FOREGROUND, "Debugger" );

  for( i = 0; i < 10; i++ ) {
    snprintf( buffer, 9, "%-3s %04x", register_names[i], *register_values[i] );
    widget_printstring( 3 + 10*(i%2), 4 + i/2, WIDGET_COLOUR_FOREGROUND,
			buffer );
  }

  widget_printstring( 3, 10, WIDGET_COLOUR_FOREGROUND, "(S)tep" );
  widget_printstring( 3, 11, WIDGET_COLOUR_FOREGROUND, "(C)ontinue" );

  widget_display_lines( 2, 11 );

  return 0;
}

void
widget_debugger_keyhandler( keyboard_key_name key, keyboard_key_name key2 )
{
  switch( key ) {

  case KEYBOARD_Resize:		/* Fake keypress used on window resize */
    widget_debugger_draw( NULL );
    break;

  case KEYBOARD_1: /* 1 used as `Escape' generates `Edit', which is Caps + 1 */
    if( key2 == KEYBOARD_Caps ) {
      debugger_run();
      widget_end_widget( WIDGET_FINISHED_CANCEL );
    }
    break;

  case KEYBOARD_c:
    debugger_run();
    widget_end_all( WIDGET_FINISHED_OK );
    break;

  case KEYBOARD_s:
    debugger_mode = DEBUGGER_MODE_HALTED;
    widget_end_all( WIDGET_FINISHED_OK );
    break;

  default:	/* Keep gcc happy */
    break;

  }
}

int ui_debugger_disassemble( libspectrum_word address GCC_UNUSED )
{
  return 0;
}

int ui_debugger_update( void )
{
  return 0;
}

#endif				/* #ifdef USE_WIDGET */
