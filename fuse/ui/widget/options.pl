#!/usr/bin/perl -w

# options.pl: generate options dialog boxes
# Copyright (c) 2001-2008 Philip Kendall, Fredrick Meunier

# $Id$

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

# Author contact information:

# E-mail: philip-fuse@shadowmagic.org.uk

use strict;

use lib '../perl';

use Fuse;
use Fuse::Dialog;

die "No data file specified" unless @ARGV;

my @dialogs = Fuse::Dialog::read( shift @ARGV );

print Fuse::GPL( 'options.c: options dialog boxes',
		 '2001-2004 Philip Kendall' ) . << "CODE";

/* This file is autogenerated from options.dat by options.pl.
   Do not edit unless you know what you\'re doing! */

#include <config.h>

#ifdef USE_WIDGET

#include <stdio.h>
#include <string.h>

#include "display.h"
#include "fuse.h"
#include "options.h"
#include "periph.h"
#include "widget_internals.h"
#include "ui/uidisplay.h"

struct widget_option_entry;

/* A generic click function */
typedef void (*widget_option_click_fn)( void );
typedef void (*widget_option_draw_fn)( int left_edge, int width, struct widget_option_entry *menu, settings_info *show );

/* A general menu */
typedef struct widget_option_entry {
  int index;
  const char *text;
  input_key key;		/* Which key to activate this option */
  const char *suffix;

  widget_option_click_fn click;
  widget_option_draw_fn draw;
} widget_option_entry;

CODE

foreach( @dialogs ) {
    foreach my $widget ( @{ $_->{widgets} } ) {
	if( $widget->{type} eq "Checkbox" or $widget->{type} eq "Entry" ) {
	    print << "CODE";
static void widget_$widget->{value}_click( void );
static void widget_option_$widget->{value}_draw( int left_edge, int width, struct widget_option_entry *menu, settings_info *show );
CODE
	} else {
	    die "Unknown type `$widget->{type}'";
	}
    }
}

foreach( @dialogs ) {

    print << "CODE";

static widget_option_entry options_$_->{name}\[\] = \{
CODE

    my $which = 0;
    foreach my $widget ( @{ $_->{widgets} } ) {

        my $text = $widget->{text};
        $text =~ s/\((.)\)/\\012$1\\001/;

	if( $widget->{type} eq "Checkbox" ) {

	    print << "CODE";
  \{ $which, "$text", $widget->{key}, NULL, widget_$widget->{value}_click, widget_option_$widget->{value}_draw \},
CODE
        } elsif( $widget->{type} eq "Entry" ) {

	    print << "CODE";
  \{ $which, "$text", $widget->{key}, "$widget->{data2}", widget_$widget->{value}_click, widget_option_$widget->{value}_draw \},
CODE
	} else {
	    die "Unknown type `$widget->{type}'";
	}

	$which++;
    }

    print << "CODE";
  \{ -1 \}
\};
CODE

}

print << "CODE";

static size_t highlight_line = 0;

/* General functions used by the options dialogs */
settings_info widget_options_settings;
int widget_options_print_option( int left_edge, int width, int number, const char* string, int value );
int widget_options_print_value( int left_edge, int width, int number, int value );
int widget_options_print_entry( int left_edge, int width, int number, const char *prefix, int value,
				const char *suffix );

static int widget_options_print_label( int left_edge, int width, int number, const char *string );
static int widget_options_print_data( int left_edge, int menu_width, int number, const char *string );

int
widget_options_print_option( int left_edge, int width, int number, const char* string, int value )
{
  widget_options_print_label( left_edge, width, number, string );
  widget_options_print_value( left_edge, width, number, value );
  return 0;
}

static int
widget_options_print_label( int left_edge, int width, int number, const char *string )
{
  char buffer[128];
  size_t l, w;
  int colour = WIDGET_COLOUR_BACKGROUND;

  if( number == highlight_line ) colour = WIDGET_COLOUR_HIGHLIGHT;
  widget_rectangle( left_edge*8+1, number*8+24, width*8-2, 1*8, colour );

  snprintf( buffer, sizeof( buffer ), "%s", string );
  l = strlen( buffer );

  if( l >= sizeof( buffer ) )
    l = sizeof( buffer ) - 1;
  while( ( w = widget_substringwidth( string, l ) ) >= (left_edge+width-2)*8 )
    --l;
  buffer[l] = '\\0';
  w = widget_printstring( left_edge*8+8, number*8+24, WIDGET_COLOUR_FOREGROUND, buffer )
      - 1;
  while ((w += 3) < (left_edge+width-1)*8-2)
    widget_putpixel (w, number*8+31, 0);
  return 0;
}

int
widget_options_print_value( int left_edge, int width, int number, int value )
{
  int colour = WIDGET_COLOUR_BACKGROUND;
  int x = (left_edge+width-2)*8-2;
  int y = number * 8 + 24;

  if( number == highlight_line ) colour = WIDGET_COLOUR_HIGHLIGHT;
  widget_rectangle( x, y, 8, 8, colour );
  widget_print_checkbox( x, y, colour, value );
  widget_display_rasters( y, 8 );
  return 0;
}

static int
widget_options_print_data( int left_edge, int menu_width, int number, const char *string )
{
  int colour = WIDGET_COLOUR_BACKGROUND;
  size_t width = widget_stringwidth( string );
  int x = (left_edge + menu_width-1)*8 - width - 2;
  int y = number * 8 + 24;

  if( number == highlight_line ) colour = WIDGET_COLOUR_HIGHLIGHT;
  widget_rectangle( x, y, width, 8, colour );
  widget_printstring( x, y, WIDGET_COLOUR_FOREGROUND, string );
  widget_display_rasters( y, 8 );

  return 0;
}

int
widget_options_print_entry( int left_edge, int width, int number, const char *prefix, int value,
			    const char *suffix )
{
  char buffer[128];
  widget_options_print_label( left_edge, width, number, prefix );
  snprintf( buffer, sizeof( buffer ), "%d %s", value, suffix );
  return widget_options_print_data( left_edge, width, number, buffer );
}

int
widget_options_finish( widget_finish_state finished )
{
  int error;

  /* If we exited normally, actually set the options */
  if( finished == WIDGET_FINISHED_OK ) {
    error = settings_copy( &settings_current, &widget_options_settings );
    settings_free( &widget_options_settings );
    memset( &widget_options_settings, 0, sizeof( settings_info ) );
    if( error ) return error;

    /* Bring the peripherals list into sync with the new options */
    periph_update();
    /* make the needed UI changes */
    uidisplay_hotswap_gfx_mode();
  }

  return 0;
}

const int options_vert_external_margin = 8;

int
widget_calculate_option_width(widget_option_entry *menu)
{
  widget_option_entry *ptr;
  int max_width=0;

  if (!menu) {
    return 64;
  }

  max_width = widget_stringwidth( menu->text )+5*8;

  for( ptr = &menu[1]; ptr->text; ptr++ ) {
    int total_width = widget_stringwidth(ptr->text)+3*8;

    /* If this is a number format, leave room for 4 digits as well as the
       text */
    if( ptr->suffix ) total_width += widget_stringwidth(ptr->suffix)+4*8;

    if (total_width > max_width)
      max_width = total_width;
  }

  return (max_width+options_vert_external_margin*2)/8;
}

CODE

foreach( @dialogs ) { 

    my $count = @{ $_->{widgets} };

    print << "CODE";
static int widget_$_->{name}_show_all( settings_info *show );

int
widget_$_->{name}_draw( void *data GCC_UNUSED )
{
  int error;
  highlight_line = 0;
  
  /* Get a copy of the current settings */
  error = settings_copy( &widget_options_settings, &settings_current );
  if( error ) { settings_free( &widget_options_settings ); return error; }

  error = widget_$_->{name}_show_all( &widget_options_settings );
  if( error ) { settings_free( &widget_options_settings ); return error; }

  widget_display_lines( 2, 2 + $count );

  return 0;
}

static int
widget_$_->{name}_show_all( settings_info *show )
\{
  widget_option_entry *ptr;
  int menu_width = widget_calculate_option_width( options_$_->{name} );
  int menu_left_edge_x;

  menu_left_edge_x = DISPLAY_WIDTH_COLS/2-menu_width/2;

  /* Draw the dialog box */
  widget_dialog_with_border( menu_left_edge_x, 2, menu_width, 2 + $count );

CODE

  my $title = $_->{title};
  $title =~ s/\\0[01][12]//g;

    print << "CODE";
  widget_printstring( menu_left_edge_x*8+2, 16, WIDGET_COLOUR_TITLE, \"$title\" );

  for( ptr=&options_$_->{name}\[0\]; ptr->index != -1; ptr++ ) \{
    ptr->draw( menu_left_edge_x, menu_width, ptr, show );
  \}

  return 0;
\}

CODE

    my $which = 0;
    foreach my $widget ( @{ $_->{widgets} } ) {

	if( $widget->{type} eq "Checkbox" ) {

	    print << "CODE";
static void
widget_$widget->{value}_click( void )
\{
  widget_options_settings.$widget->{value} = ! widget_options_settings.$widget->{value};
\}

CODE
	} elsif( $widget->{type} eq "Entry" ) {

	    my $title = $widget->{text};
            $title =~ s/\((.)\)/$1/;

	    print << "CODE";
static void
widget_$widget->{value}_click( void )
\{
  int error;
  widget_text_t text_data;

  text_data.title = "$title";
  text_data.allow = WIDGET_INPUT_DIGIT;
  snprintf( text_data.text, 40, "%d",
            widget_options_settings.$widget->{value} );
  error = widget_do( WIDGET_TYPE_TEXT, &text_data );
\}

CODE
	} else {
	    die "Unknown type `$widget->{type}'";
	}

        $widget->{text} =~ s/\((.)\)/\\012$1\\001/;

	if( $widget->{type} eq "Checkbox" ) {

	    print << "CODE";
static void
widget_option_$widget->{value}_draw( int left_edge, int width, struct widget_option_entry *menu, settings_info *show )
\{
  widget_options_print_option( left_edge, width, menu->index, menu->text, show->$widget->{value} );
\}

CODE
	} elsif( $widget->{type} eq "Entry" ) {

	    print << "CODE";
static void
widget_option_$widget->{value}_draw( int left_edge, int width, struct widget_option_entry *menu, settings_info *show )
\{
  widget_options_print_entry( left_edge, width, menu->index, "$widget->{text}", show->$widget->{value},
                              "$widget->{data2}" );
\}

CODE
	} else {
	    die "Unknown type `$widget->{type}'";
	}

	$which++;
    }

    print << "CODE";
void
widget_$_->{name}_keyhandler( input_key key )
\{
  widget_text_t text_data;
  int new_highlight_line = 0;
  int cursor_pressed = 0;
  widget_option_entry *ptr;
  int menu_width = widget_calculate_option_width(options_$_->{name});
  int menu_left_edge_x = DISPLAY_WIDTH_COLS/2-menu_width/2;

  text_data = text_data;	/* Keep gcc happy */

  switch( key ) \{

#if 0
  case INPUT_KEY_Resize:	/* Fake keypress used on window resize */
    widget_dialog_with_border( 1, 2, 30, 2 + $count );
    widget_$_->{name}_show_all( &widget_options_settings );
    break;
#endif
    
  case INPUT_KEY_Escape:
    widget_end_widget( WIDGET_FINISHED_CANCEL );
    break;

  case INPUT_KEY_Up:
  case INPUT_KEY_7:
    if ( highlight_line ) {
      new_highlight_line = highlight_line - 1;
      cursor_pressed = 1;
    }
    break;

  case INPUT_KEY_Down:
  case INPUT_KEY_6:
    if ( highlight_line + 1 < $count ) {
      new_highlight_line = highlight_line + 1;
      cursor_pressed = 1;
    }
    break;

  case INPUT_KEY_space:
  case INPUT_KEY_0:
    options_$_->{name}\[highlight_line\].click();
    options_$_->{name}\[highlight_line\].draw( menu_left_edge_x, menu_width, options_$_->{name} + highlight_line, &widget_options_settings );
    return;
    break;

  case INPUT_KEY_Return:
    widget_end_all( WIDGET_FINISHED_OK );
CODE
    print "    $_->{posthook}();\n" if $_->{posthook};
    print << "CODE";
    display_refresh_all();
    return;
    break;

  default:	/* Keep gcc happy */
    break;

  \}

  if( cursor_pressed ) \{
    int old_highlight_line = highlight_line;
    highlight_line = new_highlight_line;
    options_$_->{name}\[old_highlight_line\].draw( menu_left_edge_x, menu_width, options_$_->{name} + old_highlight_line, &widget_options_settings );
    options_$_->{name}\[highlight_line\].draw( menu_left_edge_x, menu_width, options_$_->{name} + highlight_line, &widget_options_settings );
    return;
  \}

  for( ptr=&options_$_->{name}\[0\]; ptr->index != -1; ptr++ ) \{
    if( key == ptr->key ) \{
      int old_highlight_line = highlight_line;
      ptr->click();
      highlight_line = ptr->index;
      options_$_->{name}\[old_highlight_line\].draw( menu_left_edge_x, menu_width, options_$_->{name} + old_highlight_line, &widget_options_settings );
      ptr->draw( menu_left_edge_x, menu_width, ptr, &widget_options_settings );
      break;
    \}
  \}
\}
CODE

}

print "\n#endif				/* #ifdef USE_WIDGET */\n";

