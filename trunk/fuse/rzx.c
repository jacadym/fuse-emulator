/* rzx.c: .rzx files
   Copyright (c) 2002 Philip Kendall

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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>

#include "event.h"
#include "fuse.h"
#include "keyboard.h"
#include "machine.h"
#include "libspectrum/rzx.h"
#include "rzx.h"
#include "types.h"
#include "ui/ui.h"
#include "utils.h"
#include "z80/z80_macros.h"

/* The count of instruction fetches needed for .rzx files */
size_t rzx_instructions;

/* Are we currently recording a .rzx file? */
int rzx_recording;

/* The filename we'll save this recording into */
static char *rzx_filename;

/* Are we currently playing back a .rzx file? */
int rzx_playback;

/* The .rzx frame we're currently playing */
size_t rzx_current_frame;

/* The current RZX data */
libspectrum_rzx rzx;

static int recording_frame( void );
static int playback_frame( void );

int rzx_init( void )
{
  rzx_recording = rzx_playback = 0;

  return 0;
}

int rzx_start_recording( const char *filename )
{
  libspectrum_error error;

  size_t i; libspectrum_byte keyboard[8];

  if( rzx_playback ) return 1;

  /* Note that we're recording */
  rzx_recording = 1;

  for( i=0; i<8; i++ )
    keyboard[i] = keyboard_default_value & keyboard_return_values[i];

  error = libspectrum_rzx_frame( &rzx, 0, keyboard );
  if( error ) return error;

  /* Start the count of instruction fetches here */
  rzx_instructions = 0;

  /* Store the filename */
  rzx_filename = (char*)malloc( strlen(filename) + 1 );
  if( rzx_filename == NULL ) {
    ui_error( "out of memory in rzx_start_recording" );
    return 1;
  }
  strcpy( rzx_filename, filename );

  return 0;
}

int rzx_stop_recording( void )
{
  libspectrum_byte *buffer; size_t length;
  libspectrum_error libspec_error; int error;

  /* Stop recording data */
  rzx_recording = 0;

  length = 0;
  libspec_error = libspectrum_rzx_write( &rzx, &buffer, &length );
  if( libspec_error != LIBSPECTRUM_ERROR_NONE ) {
    ui_error( "error during libspectrum_rzx_write: %s",
	      libspectrum_error_message( libspec_error ) );
    libspectrum_rzx_free( &rzx );
    return libspec_error;
  }

  error = utils_write_file( rzx_filename, buffer, length );
  free( rzx_filename );
  if( error ) { free( buffer ); libspectrum_rzx_free( &rzx ); return error; }

  free( buffer );

  libspec_error = libspectrum_rzx_free( &rzx );
  if( libspec_error != LIBSPECTRUM_ERROR_NONE ) {
    ui_error( "error during libspectrum_rzx_free: %s",
	      libspectrum_error_message( libspec_error ) );
    return libspec_error;
  }

  return 0;
}

int rzx_start_playback( const char *filename )
{
  libspectrum_byte *buffer; size_t length;
  libspectrum_error libspec_error; int error;

  if( rzx_recording) return 1;

  error = utils_read_file( filename, &buffer, &length );
  if( error ) return error;

  libspec_error = libspectrum_rzx_read( &rzx, buffer, length );
  if( libspec_error != LIBSPECTRUM_ERROR_NONE ) {
    ui_error( "error during libspectrum_rzx_read: %s",
	      libspectrum_error_message( libspec_error ) );
    munmap( buffer, length );
    return libspec_error;
  }

  if( munmap( buffer, length ) == -1 ) {
    ui_error( "Couldn't munmap '%s': %s", filename, strerror( errno ) );
    return 1;
  }

  /* Interrupts will now be generated by the RZX code */
  error = event_remove_type( EVENT_TYPE_INTERRUPT );
  if( error ) return error;

  /* We're now playing this RZX file */
  rzx_playback = 1;
  rzx_current_frame = 0; rzx_instructions = 0;

  return 0;
}

int rzx_stop_playback( void )
{
  libspectrum_error libspec_error; int error;

  rzx_playback = 0;

  /* We've now finished with the RZX file, so add an interrupt event
     back in */
  error = event_add( machine_current->timings.cycles_per_frame,
		     EVENT_TYPE_INTERRUPT );
  if( error ) return error;

  libspec_error = libspectrum_rzx_free( &rzx );
  if( libspec_error != LIBSPECTRUM_ERROR_NONE ) {
    ui_error( "libspec_error during libspectrum_rzx_free: %s",
	      libspectrum_error_message( libspec_error ) );
    return libspec_error;
  }

  return 0;
}  

int rzx_frame( void )
{
  if( rzx_recording ) return recording_frame();
  if( rzx_playback  ) return playback_frame();
  return 0;
}

static int recording_frame( void )
{
  libspectrum_error error;

  size_t i; libspectrum_byte keyboard[8];

  for( i=0; i<8; i++ )
    keyboard[i] = keyboard_default_value & keyboard_return_values[i];

  error = libspectrum_rzx_frame( &rzx, rzx_instructions, keyboard );
  if( error ) return error;

  /* Reset the instruction counter */
  rzx_instructions = 0;

  return 0;
}

static int playback_frame( void )
{
  /* Increment the frame count and see if we've finished with this file */
  if( ++rzx_current_frame >= rzx.count ) return rzx_stop_playback();

  /* If we've got more frame to do, just reset the count and continue */
  rzx_instructions = 0;

  return 0;
}

int rzx_end( void )
{
  if( rzx_recording ) return rzx_stop_recording();
  if( rzx_playback  ) return rzx_stop_playback();
  return 0;
}
