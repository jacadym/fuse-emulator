/* menu.c: general menu widget
   Copyright (c) 2001,2002 Philip Kendall

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

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>

#include "display.h"
#include "fuse.h"
#include "machine.h"
#include "rzx.h"
#include "snapshot.h"
#include "tape.h"
#include "utils.h"
#include "widget.h"
#include "ui/uidisplay.h"

#define ERROR_MESSAGE_MAX_LENGTH 1024

widget_menu_entry *menu;

int widget_menu_draw( void *data )
{
  widget_menu_entry *ptr;
  size_t menu_entries, i;

  menu = (widget_menu_entry*)data;

  /* How many menu items do we have? */
  for( ptr = &menu[1]; ptr->text; ptr++ ) ;
  menu_entries = ptr - &menu[1];

  widget_dialog_with_border( 1, 2, 30, menu_entries + 2 );

  widget_printstring( 15 - strlen( menu->text ) / 2, 2,
		      WIDGET_COLOUR_FOREGROUND, menu->text );

  for( i=0; i<menu_entries; i++ )
    widget_printstring( 2, i+4, WIDGET_COLOUR_FOREGROUND,
			menu[i+1].text );

  uidisplay_lines( DISPLAY_BORDER_HEIGHT + 16,
		   DISPLAY_BORDER_HEIGHT + 16 + (menu_entries+2)*8 );

  return 0;
}

void widget_menu_keyhandler( keyboard_key_name key )
{
  widget_menu_entry *ptr;

  switch( key ) {
    
  case KEYBOARD_1: /* 1 used as `Escape' generates `Edit', which is Caps + 1 */
    widget_return[ widget_level ].finished = WIDGET_FINISHED_CANCEL;
    return;

  case KEYBOARD_Enter:
    widget_return[ widget_level ].finished = WIDGET_FINISHED_OK;
    return;

  default:	/* Keep gcc happy */
    break;

  }

  for( ptr=&menu[1]; ptr->text; ptr++ ) {
    if( key == ptr->key ) {
      (ptr->action)( ptr->data );	/* function pointer call */
      break;
    }
  }
}

/* General callbacks */

/* The callback used to call another widget */
int widget_menu_widget( void *data )
{
  widget_menu_widget_t *ptr = (widget_menu_widget_t*)data;

  return widget_do( ptr->widget, ptr->data );
}

/* The callback to get a file name and do something with it */
int widget_apply_to_file( void *data )
{
  typedef int (*fn_ptr)(const char*);

  fn_ptr ptr = (fn_ptr)data;

  widget_do( WIDGET_TYPE_FILESELECTOR, NULL );
  if( widget_filesel_name ) ptr( widget_filesel_name );

  return 0;
}

/* File menu callbacks */

/* File/Save */
int widget_menu_save_snapshot( void *data )
{
  widget_end_all( WIDGET_FINISHED_OK );
  return snapshot_write( "snapshot.z80" );
}

/* File/Recording/Stop */
int widget_menu_rzx_stop( void *data )
{
  if( rzx_recording ) rzx_stop_recording();
  if( rzx_playback  ) rzx_stop_playback();
  return 0;
}  

/* File/Exit */
int widget_menu_exit( void *data )
{
  fuse_exiting = 1;
  widget_end_all( WIDGET_FINISHED_OK );
  return 0;
}

/* Machine/Reset */
int widget_menu_reset( void *data )
{
  widget_end_all( WIDGET_FINISHED_OK );
  return machine_current->reset();
}

/* Tape/Play */
int widget_menu_play_tape( void *data )
{
  widget_end_all( WIDGET_FINISHED_OK );
  return tape_toggle_play();
}

/* Tape/Rewind */
int widget_menu_rewind_tape( void *data )
{
  widget_end_all( WIDGET_FINISHED_OK );
  return tape_rewind();
}

/* Tape/Clear */
int widget_menu_clear_tape( void *data )
{
  widget_end_all( WIDGET_FINISHED_OK );
  return tape_close();
}

/* Tape/Write */
int widget_menu_write_tape( void *data )
{
  widget_end_all( WIDGET_FINISHED_OK );
  return tape_write( "tape.tzx" );
}

/* Help/Keyboard Picture */
int widget_menu_keyboard( void *data )
{
  widget_picture_data *ptr = (widget_picture_data*)data;

  int error, fd;
  size_t length;

  char error_message[ ERROR_MESSAGE_MAX_LENGTH ];

  fd = utils_find_lib( ptr->filename );
  if( fd == -1 ) {
    fprintf( stderr, "%s: couldn't find keyboard picture (`%s')\n",
	     fuse_progname, ptr->filename );
    return 1;
  }
  
  error = utils_read_fd( fd, ptr->filename, &(ptr->screen), &length );
  if( error ) return error;

  if( length != 6912 ) {
    fprintf( stderr, "%s: keyboard picture (`%s') is not 6912 bytes long\n",
	     fuse_progname, ptr->filename );
    return 1;
  }

  widget_do( WIDGET_TYPE_PICTURE, ptr );

  if( munmap( ptr->screen, length ) == -1 ) {
    snprintf( error_message, ERROR_MESSAGE_MAX_LENGTH,
	      "%s: Couldn't munmap keyboard picture (`%s')",
	      fuse_progname, ptr->filename );
    perror( error_message );
    return 1;
  }

  return 0;
}
