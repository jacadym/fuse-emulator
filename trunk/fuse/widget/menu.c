/* menu.c: general menu widget
   Copyright (c) 2001-2004 Philip Kendall

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

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "dck.h"
#include "debugger/debugger.h"
#include "event.h"
#include "fuse.h"
#include "joystick.h"
#include "machine.h"
#include "machines/specplus3.h"
#include "menu.h"
#include "psg.h"
#include "rzx.h"
#include "screenshot.h"
#include "settings.h"
#include "simpleide.h"
#include "snapshot.h"
#include "tape.h"
#include "trdos.h"
#include "ui/uidisplay.h"
#include "utils.h"
#include "widget_internals.h"

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

  for( i = 0; i < menu_entries; i++ ) {
    int colour = menu[i+1].inactive ?
                 WIDGET_COLOUR_DISABLED :
		 WIDGET_COLOUR_FOREGROUND;
    widget_printstring( 2, i+4, colour, menu[i+1].text );
  }

  widget_display_lines( 2, menu_entries + 2 );

  return 0;
}

void
widget_menu_keyhandler( input_key key )
{
  widget_menu_entry *ptr;

  switch( key ) {

#if 0
  case INPUT_KEY_Resize:	/* Fake keypress used on window resize */
    widget_menu_draw( menu );
    break;
#endif
    
  case INPUT_KEY_Escape:
    widget_end_widget( WIDGET_FINISHED_CANCEL );
    return;

  case INPUT_KEY_Return:
    widget_end_widget( WIDGET_FINISHED_OK );
    return;

  default:	/* Keep gcc happy */
    break;

  }

  for( ptr=&menu[1]; ptr->text; ptr++ ) {
    if( !ptr->inactive && key == ptr->key ) {

      if( ptr->submenu ) {
	widget_do( WIDGET_TYPE_MENU, ptr->submenu );
      } else {
	ptr->callback( ptr->action );
      }

      break;
    }
  }
}

/* General callbacks */

char *
menu_get_filename( const char *title )
{
  char *filename = NULL;

  widget_do( WIDGET_TYPE_FILESELECTOR, NULL );
  if( widget_filesel_name ) {
    filename = strdup( widget_filesel_name );
    if( !filename )
      ui_error( UI_ERROR_ERROR, "Out of memory at %s:%d", __FILE__, __LINE__ );
  }

  return filename;
}

void
menu_file_savesnapshot( int action )
{
  widget_end_all( WIDGET_FINISHED_OK );

  if( settings_current.snaps_as_z80 ) {
    snapshot_write( "snapshot.z80" );
  } else {
    snapshot_write( "snapshot.szx" );
  }

}

void
menu_file_recording_record( int action )
{
  if( rzx_playback || rzx_recording ) return;

  widget_end_all( WIDGET_FINISHED_OK );
  rzx_start_recording( "record.rzx", 1 );
}

void
menu_file_recording_recordfromsnapshot( int action )
{
  int error;

  if( rzx_playback || rzx_recording ) return;

  /* Get a snapshot name */
  widget_do( WIDGET_TYPE_FILESELECTOR, NULL );

  if( !widget_filesel_name ) widget_end_widget( WIDGET_FINISHED_CANCEL );

  error = snapshot_read( widget_filesel_name );
  if( error ) {
    if( widget_filesel_name ) free( widget_filesel_name );
    return;
  }

  rzx_start_recording( "record.rzx", settings_current.embed_snapshot );
}

void
menu_file_aylogging_record( int action )
{
  if( psg_recording ) return;

  widget_end_all( WIDGET_FINISHED_OK );
  psg_start_recording( "ay.psg" );
}

void
menu_file_savescreenasscr( int action )
{
  widget_end_all( WIDGET_FINISHED_OK );
  screenshot_scr_write( "fuse.scr" );
}

void
menu_file_movies_recordmovieasscr( int action )
{
  widget_end_all( WIDGET_FINISHED_OK );
  screenshot_movie_record = 1;
}

#ifdef USE_LIBPNG
void
menu_file_savescreenaspng( int action )
{
  scaler_type scaler;

  scaler = menu_get_scaler( screenshot_available_scalers );
  if( scaler == SCALER_NUM ) return;

  screenshot_write( "fuse.png", scaler );
}

void
menu_file_movies_recordmovieaspng( int action )
{
  scaler_type scaler;

  scaler = menu_get_scaler( screenshot_available_scalers );
  if( scaler == SCALER_NUM ) return;
  
  screenshot_movie_scaler = scaler;
  screenshot_movie_record = 2;
}
#endif			/* #ifdef USE_LIBPNG */

scaler_type
menu_get_scaler( scaler_available_fn selector )
{
  size_t count, i;
  const char *options[ SCALER_NUM ];
  widget_select_t info;
  int error;

  count = 0; info.current = 0;

  for( i = 0; i < SCALER_NUM; i++ )
    if( selector( i ) ) {
      if( current_scaler == i ) info.current = count;
      options[ count++ ] = scaler_name( i );
    }

  info.title = "Select scaler";
  info.options = options;
  info.count = count;

  error = widget_do( WIDGET_TYPE_SELECT, &info );
  if( error ) return SCALER_NUM;

  if( info.result == -1 ) return SCALER_NUM;

  for( i = 0; i < SCALER_NUM; i++ )
    if( selector( i ) && !info.result-- ) return i;

  ui_error( UI_ERROR_ERROR, "widget_select_scaler: ran out of scalers" );
  fuse_abort();
}

void
menu_file_exit( int action )
{
  fuse_exiting = 1;
  widget_end_all( WIDGET_FINISHED_OK );
}

void
menu_options_general( int action )
{
  widget_do( WIDGET_TYPE_GENERAL, NULL );
}

void
menu_options_peripherals( int action )
{
  widget_do( WIDGET_TYPE_PERIPHERALS, NULL );
}

void
menu_options_sound( int action )
{
  widget_do( WIDGET_TYPE_SOUND, NULL );
}

void
menu_options_rzx( int action )
{
  widget_do( WIDGET_TYPE_RZX, NULL );
}

void
menu_options_joysticks_select( int action )
{
  widget_select_t info;
  int *setting, error;

  setting = NULL;

  switch( action - 1 ) {

  case 0: setting = &( settings_current.joystick_1_output ); break;
  case 1: setting = &( settings_current.joystick_2_output ); break;
  case JOYSTICK_KEYBOARD:
    setting = &( settings_current.joystick_keyboard_output ); break;

  }

  info.title = "Select joystick";
  info.options = joystick_name;
  info.count = JOYSTICK_TYPE_COUNT;
  info.current = *setting;

  error = widget_do( WIDGET_TYPE_SELECT, &info );
  if( error ) return;

  if( info.result != -1 ) *setting = info.result;
}

/* Options/Select ROMs/<type> */
int
menu_select_roms( libspectrum_machine machine, size_t start, size_t count )
{
  widget_roms_info info;

  info.machine = machine;
  info.start = start;
  info.count = count;
  info.initialised = 0;

  return widget_do( WIDGET_TYPE_ROM, &info );
}

void
menu_machine_reset( int action )
{
  widget_end_all( WIDGET_FINISHED_OK );
  machine_reset();
}

void
menu_machine_select( int action )
{
  widget_select_t info;
  char **options, *buffer;
  size_t i;
  int error;
  libspectrum_machine new_machine;

  options = malloc( machine_count * sizeof( const char * ) );
  if( !options ) {
    ui_error( UI_ERROR_ERROR, "out of memory at %s:%d", __FILE__, __LINE__ );
    return;
  }

  buffer = malloc( 40 * machine_count );
  if( !buffer ) {
    ui_error( UI_ERROR_ERROR, "out of memory at %s:%d", __FILE__, __LINE__ );
    free( options );
    return;
  }

  for( i = 0; i < machine_count; i++ ) {
    options[i] = &buffer[ i * 40 ];
    snprintf( options[i], 40,
	      libspectrum_machine_name( machine_types[i]->machine ) );
    if( machine_current->machine == machine_types[i]->machine )
      info.current = i;
  }

  info.title = "Select machine";
  info.options = (const char**)options;
  info.count = machine_count;

  error = widget_do( WIDGET_TYPE_SELECT, &info );
  free( buffer ); free( options );
  if( error ) return;

  if( info.result == -1 ) return;

  new_machine = machine_types[ info.result ]->machine;

  if( machine_current->machine != new_machine ) machine_select( new_machine );
}

void
menu_machine_debugger( int action )
{
  debugger_mode = DEBUGGER_MODE_HALTED;
  widget_do( WIDGET_TYPE_DEBUGGER, NULL );
}

void
menu_media_tape_browse( int action )
{
  widget_do( WIDGET_TYPE_BROWSE, NULL );
}

int
ui_tape_write( void )
{
  widget_end_all( WIDGET_FINISHED_OK );
  return tape_write( "tape.tzx" );
}

#ifdef HAVE_765_H
int
ui_plus3_disk_write( specplus3_drive_number which )
{
  char filename[ 20 ];

  snprintf( filename, 20, "drive%c.dsk", (char)( 'a' + which ) );

  return specplus3_disk_write( which, filename );
}
#endif				/* #ifdef HAVE_765_H */

int
ui_trdos_disk_write( trdos_drive_number which )
{
  char filename[ 20 ];

  snprintf( filename, 20, "drive%c.trd", (char)( 'a' + which ) );

  return trdos_disk_write( which, filename );
}

void
menu_help_keyboard( int action )
{
  int error, fd;
  utils_file file;
  widget_picture_data info;

  static const char *filename = "keyboard.scr";

  fd = utils_find_auxiliary_file( filename, UTILS_AUXILIARY_LIB );
  if( fd == -1 ) {
    ui_error( UI_ERROR_ERROR, "couldn't find keyboard picture ('%s')",
	      filename );
    return;
  }
  
  error = utils_read_fd( fd, filename, &file ); if( error ) return;

  if( file.length != 6912 ) {
    ui_error( UI_ERROR_ERROR, "keyboard picture ('%s') is not 6912 bytes long",
	      filename );
    utils_close_file( &file );
    return;
  }

  info.filename = filename;
  info.screen = file.buffer;

  widget_do( WIDGET_TYPE_PICTURE, &info );

  if( utils_close_file( &file ) ) return;
}

static int
set_active( struct widget_menu_entry *menu, const char *path, int active )
{
  if( *path == '/' ) path++;

  /* Skip the menu title */
  menu++;

  for( ; menu->text; menu++ ) {

    const char *p = menu->text, *q = path;

    /* Compare the two strings, but skip and '(' and ')' characters
       in the menu (used to designate the hotkey) */
    do {
      if( *p == '(' || *p == ')' ) p++;
    } while( *p && *p++ == *q++ );

    if( *p ) continue;		/* not matched */

    /* match, but with a submenu */
    if( *q == '/' ) return set_active( menu->submenu, q, active );

    if( *q ) continue;		/* not matched */

    /* we have a match */
    menu->inactive = !active;
    return 0; 
  }

  return 1;
}

int
ui_menu_item_set_active( const char *path, int active )
{
  return set_active( widget_menu, path, active );
}

#endif				/* #ifdef USE_WIDGET */
