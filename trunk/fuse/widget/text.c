/* text.c: simple text entry widget
   Copyright (c) 2002-2005 Philip Kendall

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

#include <string.h>
#include <ctype.h>

#include "widget_internals.h"

char *widget_text_text = NULL;	/* What we return the text in */

static const char *title;	/* The window title */
static widget_text_input_allow allow;
static char text[40];		/* The current entry text */

static void delete_character( void );
static void append_character( char c );

int
widget_text_draw( void *data )
{
  char buffer[32];
  widget_text_t* text_data = data;

  if( data ) {
    title = text_data->title;
    allow = text_data->allow;
    snprintf( text, sizeof( text ), "%s", text_data->text );
  }

  widget_dialog_with_border( 1, 2, 30, 3 );

  widget_printstring( 15 - strlen( title ) / 2, 2, WIDGET_COLOUR_FOREGROUND,
		      title );

  strcpy( buffer, "[ " );
  strncpy( &buffer[2], text, 23 ); buffer[23] = '\0';
  strcat( buffer, "_" );

  widget_printstring(  2, 4, WIDGET_COLOUR_FOREGROUND, buffer );
  widget_printstring( 29, 4, WIDGET_COLOUR_FOREGROUND, "]" );

  widget_display_lines( 2, 3 );

  return 0;
}

void
widget_text_keyhandler( input_key key )
{
  switch( key ) {

  case INPUT_KEY_BackSpace:	/* Backspace generates DEL which is Caps + 0 */
    delete_character(); widget_text_draw( NULL );
    return;

  case INPUT_KEY_Escape:
    widget_end_widget( WIDGET_FINISHED_CANCEL );
    return;

  case INPUT_KEY_Return:
    widget_end_widget( WIDGET_FINISHED_OK );
    return;

  default:			/* Keep gcc happy */
    break;

  }

  /* Input validation.
   * We rely on various INPUT_KEY_* being mapped directly onto ASCII.
   */
 
  /* FIXME: we *don't* want keypresses filtered through the input layer */
 
  /* First, return if the character isn't printable ASCII. */
  if( key < ' ' || key > '~' ) return;
 
  /* Return if the key isn't valid. */
  switch( allow ) {
  case WIDGET_INPUT_ASCII:
    break;
  case WIDGET_INPUT_DIGIT:
    if( !isdigit( key ) ) return;
    break;
  case WIDGET_INPUT_ALPHA:
    if( !isalpha( key ) ) return;
    break;
  case WIDGET_INPUT_ALNUM:
    if( !isdigit( key ) && !isalpha( key ) ) return;
    break;
  }
  
  /* If we've got this far, we have a valid key */
  append_character( key );
 
  widget_text_draw( NULL );
}

static void 
delete_character( void )
{
  size_t length = strlen( text );

  if( length ) text[ length - 1 ] = '\0';
}

static void
append_character( char c )
{
  size_t length = strlen( text );

  if( length < 23 ) {
    text[ length ] = c; text[ length + 1 ] = '\0';
  }
}

int
widget_text_finish( widget_finish_state finished )
{
  if( finished == WIDGET_FINISHED_OK ) {

    widget_text_text = realloc( widget_text_text, strlen( text ) + 1 );
    if( !widget_text_text ) {
      ui_error( UI_ERROR_ERROR, "Out of memory at %s:%d", __FILE__, __LINE__ );
      return 1;
    }

    strcpy( widget_text_text, text );
  } else {
    free( widget_text_text );
    widget_text_text = NULL;
  }

  return 0;
}

#endif				/* #ifdef USE_WIDGET */
