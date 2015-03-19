#!/usr/bin/perl -w

# options.pl: generate options dialog boxes
# Copyright (c) 2001-2013 Philip Kendall, Fredrick Meunier

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

my %combo_sets;
my %combo_default;
my @dialogs = Fuse::Dialog::read( shift @ARGV );

print Fuse::GPL( 'options.c: options dialog boxes',
		 '2001-2013 Philip Kendall' ) . << "CODE";

/* This file is autogenerated from options.dat by options.pl.
   Do not edit unless you know what you\'re doing! */

#include <config.h>

#ifdef USE_WIDGET

#include <stdio.h>
#include <string.h>

#include "display.h"
#include "fuse.h"
#include "options.h"
#include "options_internals.h"
#include "periph.h"
#include "ui/widget/widget_internals.h"
#include "ui/uidisplay.h"

struct widget_option_entry;

/* A generic click function */
typedef void (*widget_option_click_fn)( void );
typedef void (*widget_option_draw_fn)( int left_edge, int width, struct widget_option_entry *menu, settings_info *show );

/* A general menu */
typedef struct widget_option_entry {
  const char *text;
  int index;
  input_key key;		/* Which key to activate this option */
  const char *suffix;
  const char * const *options;

  widget_option_click_fn click;
  widget_option_draw_fn draw;
} widget_option_entry;

static void
widget_combo_click( const char *title, const char * const *options, char **current, int def )
\{
  int error, i;
  widget_select_t sel;

  sel.title = title;
  sel.options = options;
  sel.current = def;
  sel.finish_all = 0;
  for( i = 0; options[i] != NULL; i++ ) {
    if( *current != NULL && !strcmp( options[ i ], *current ) )
        sel.current = i;
  }
  sel.count = i;

  error = widget_do_select( &sel );

  if( !error && sel.result >= 0 ) \{
    if( *current ) free( *current );
    *current = strdup( options[ sel.result ] );
  \}
\}

static int
option_enumerate_combo( const char * const *options, char *value, int def ) {
  int i;
  if( value != NULL ) {
    for( i = 0; options[i] != NULL; i++) {
      if( !strcmp( value, options[ i ] ) )
        return i;
    }
  }
  return def;
}

CODE

foreach( @dialogs ) {
    foreach my $widget ( @{ $_->{widgets} } ) {
	if( $widget->{type} eq "Combo" ) {
	    my $n = 0;

	    foreach( split /\|/, $widget->{data1} ) {
		if( s/^\*// ) {
		    $combo_default{$widget->{value}} = $n;
		}
		$n++;
	    }
	    $widget->{data1} =~ s/^\*//;
	    $widget->{data1} =~ s/\|\*/|/;
	    if( not exists( $combo_sets{$widget->{data1}} ) ) {
		$combo_sets{$widget->{data1}} = "widget_$widget->{value}_combo";

		print << "CODE";
static const char * const widget_$widget->{value}_combo[] = {
CODE
		foreach( split /\|/, $widget->{data1} ) {
		    print << "CODE";
  "$_",
CODE
		}
		print << "CODE";
  NULL
};

CODE
	    } else {
		print << "CODE";
\#define widget_$widget->{value}_combo $combo_sets{$widget->{data1}}

CODE
	    }
		print << "CODE";
int
option_enumerate_$_->{name}_$widget->{value}( void ) {
  return option_enumerate_combo( widget_$widget->{value}_combo,
				 settings_current.$widget->{value},
				 $combo_default{$widget->{value}} );
}

CODE
	}
    }
}

foreach( @dialogs ) {
    print "static int  widget_$_->{name}_running = 0;\n";
    foreach my $widget ( @{ $_->{widgets} } ) {
	if( $widget->{type} eq "Checkbox" or $widget->{type} eq "Entry" or $widget->{type} eq "Combo" ) {
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

  my $title = $_->{title};
  $title =~ s/\\0[01][12]//g;

    print << "CODE";

static widget_option_entry options_$_->{name}\[\] = \{
  \{ "$_->{title}" \},
CODE

    my $which = 0;
    foreach my $widget ( @{ $_->{widgets} } ) {

        my $text = $widget->{text};
        $text =~ s/\((.)\)/\\012$1\\001/;

	if( $widget->{type} eq "Checkbox" ) {

	    print << "CODE";
  \{ "$text", $which, $widget->{key}, NULL, NULL, widget_$widget->{value}_click, widget_option_$widget->{value}_draw \},
CODE
        } elsif( $widget->{type} eq "Entry" ) {

	    print << "CODE";
  \{ "$text", $which, $widget->{key}, "$widget->{data2}", NULL, widget_$widget->{value}_click, widget_option_$widget->{value}_draw \},
CODE
        } elsif( $widget->{type} eq "Combo" ) {

	    print << "CODE";
  \{ "$text", $which, $widget->{key}, NULL, widget_$widget->{value}_combo, widget_$widget->{value}_click, widget_option_$widget->{value}_draw \},
CODE
	} else {
	    die "Unknown type `$widget->{type}'";
	}

	$which++;
    }

    print << "CODE";
  \{ NULL \}
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
int widget_options_print_combo( int left_edge, int width, int number, const char *prefix,
				const char * const *options, const char *value, int def );

static int widget_options_print_label( int left_edge, int width, int number, const char *string );
static int widget_options_print_data( int left_edge, int menu_width, int number, const char *string, int tcolor );
static int widget_calculate_option_width(widget_option_entry *menu);

static int
widget_options_show_all( widget_option_entry *options, settings_info *show )
\{
  widget_option_entry *ptr;
  size_t height = 0;
  int menu_width = widget_calculate_option_width( options );
  int menu_left_edge_x;

  /* How many options do we have? */
  for( ptr = &options[1]; ptr->text; ptr++ )
    height ++;

  menu_left_edge_x = DISPLAY_WIDTH_COLS/2-menu_width/2;

  /* Draw the dialog box */
  widget_dialog_with_border( menu_left_edge_x, 2, menu_width, 2 + height );

  widget_printstring( menu_left_edge_x*8+2, 16, WIDGET_COLOUR_TITLE, options->text );

  for( ptr = &options[1]; ptr->text; ptr++ ) {
    ptr->draw( menu_left_edge_x, menu_width, ptr, show );
  }

  widget_display_lines( 2, 2 + height );

  return 0;
\}

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
widget_options_print_data( int left_edge, int menu_width, int number, const char *string, int tcolor )
{
  int colour = WIDGET_COLOUR_BACKGROUND;
  size_t width = widget_stringwidth( string );
  int x = (left_edge + menu_width-1)*8 - width - 2;
  int y = number * 8 + 24;

  if( number == highlight_line ) colour = WIDGET_COLOUR_HIGHLIGHT;
  widget_rectangle( x, y, width, 8, colour );
  widget_printstring( x, y, tcolor, string );
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
  return widget_options_print_data( left_edge, width, number, buffer, WIDGET_COLOUR_FOREGROUND );
}

int
widget_options_print_combo( int left_edge, int width, int number, const char *prefix,
			    const char * const *option, const char *value, int def )
{
  int i = 0;
  const char *c;
  char buffer[64];

  c = option[ def ];
  if( value != NULL ) {
    while( option[ i ] != NULL ) {
      if( !strcmp( option[ i ], value ) )
        c = option[ i ];
      i++;
    }
  }
  widget_options_print_label( left_edge, width, number, prefix );
  snprintf( buffer, sizeof( buffer ), "%s", c );
  return widget_options_print_data( left_edge, width, number, buffer, WIDGET_COLOUR_DISABLED );
}

int
widget_options_finish( widget_finish_state finished )
{
  int error;

  /* If we exited normally, actually set the options */
  if( finished == WIDGET_FINISHED_OK ) {
    /* Get a copy of current settings */
    settings_info original_settings;
    memset( &original_settings, 0, sizeof( settings_info ) );
    settings_copy( &original_settings, &settings_current );

    /* Apply new options */
    settings_copy( &settings_current, &widget_options_settings );

    int needs_hard_reset = periph_postcheck();

    if( needs_hard_reset ) {
      error = widget_do_query( "Some options need to reset the machine. Reset?" );
      if( !error && !widget_query.confirm )
        settings_copy( &settings_current, &original_settings );
      else
        periph_posthook();
    }
    else {
      /* Bring the peripherals list into sync with the new options */
      periph_posthook();
    }

    settings_free( &original_settings );

    /* make the needed UI changes */
    uidisplay_hotswap_gfx_mode();
  }
  settings_free( &widget_options_settings );
  memset( &widget_options_settings, 0, sizeof( settings_info ) );

  return 0;
}

const int options_vert_external_margin = 8;

static int
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
    if( ptr->options ) {
      int combo_width = 0;
      const char * const *options = ptr->options;
      while( *options != NULL ) {
        if( combo_width < widget_stringwidth( *options ) )
          combo_width = widget_stringwidth( *options );
        options++;
      }
      total_width += combo_width;
    }

    if (total_width > max_width)
      max_width = total_width;
  }

  return (max_width+options_vert_external_margin*2)/8;
}

CODE

foreach( @dialogs ) { 

    my $count = @{ $_->{widgets} };

    print << "CODE";

int
widget_$_->{name}_draw( void *data GCC_UNUSED )
{
  int error;

  if( !widget_$_->{name}_running ) {		/* we want to copy settings, only when start up */
    highlight_line = 0;
    /* Get a copy of the current settings */
    settings_copy( &widget_options_settings, &settings_current );
    widget_$_->{name}_running = 1;
  }

  error = widget_options_show_all( options_$_->{name}, &widget_options_settings );
  if( error ) { settings_free( &widget_options_settings ); return error; }

  return 0;
}

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
  widget_text_t text_data;

  text_data.title = "$title";
  text_data.allow = WIDGET_INPUT_DIGIT;
  snprintf( text_data.text, 40, "%d",
            widget_options_settings.$widget->{value} );
  widget_do_text( &text_data );

  if( widget_text_text ) \{
    widget_options_settings.$widget->{value} = atoi( widget_text_text );
  \}
\}

CODE
	} elsif( $widget->{type} eq "Combo" ) {

	    my $title = $widget->{text};
            $title =~ s/\((.)\)/$1/;

	    print << "CODE";
static void
widget_$widget->{value}_click( void )
\{
  widget_combo_click( "$title", widget_$widget->{value}_combo,
				&widget_options_settings.$widget->{value},
				$combo_default{$widget->{value}} );
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
  widget_options_print_entry( left_edge, width, menu->index, menu->text, show->$widget->{value},
                              menu->suffix );
\}

CODE
	} elsif( $widget->{type} eq "Combo" ) {

	    print << "CODE";
static void
widget_option_$widget->{value}_draw( int left_edge, int width, struct widget_option_entry *menu, settings_info *show )
\{
  widget_options_print_combo( left_edge, width, menu->index, menu->text, menu->options,
			      show->$widget->{value}, $combo_default{$widget->{value}} );
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
  case INPUT_JOYSTICK_FIRE_2:
    widget_end_widget( WIDGET_FINISHED_CANCEL );
    widget_$_->{name}_running = 0;
    break;

  case INPUT_KEY_Up:
  case INPUT_KEY_7:
  case INPUT_JOYSTICK_UP:
    if ( highlight_line ) {
      new_highlight_line = highlight_line - 1;
      cursor_pressed = 1;
    }
    break;

  case INPUT_KEY_Down:
  case INPUT_KEY_6:
  case INPUT_JOYSTICK_DOWN:
    if ( highlight_line + 1 < $count ) {
      new_highlight_line = highlight_line + 1;
      cursor_pressed = 1;
    }
    break;

  case INPUT_KEY_Home:
    if ( highlight_line ) {
      new_highlight_line = 0;
      cursor_pressed = 1;
    }
    break;

  case INPUT_KEY_End:
    if ( highlight_line + 2 < $count ) {
      new_highlight_line = $count - 1;
      cursor_pressed = 1;
    }
    break;

  case INPUT_KEY_space:
  case INPUT_KEY_0:
  case INPUT_JOYSTICK_RIGHT:
    options_$_->{name}\[highlight_line+1\].click();
    options_$_->{name}\[highlight_line+1\].draw( menu_left_edge_x, menu_width, options_$_->{name} + highlight_line + 1, &widget_options_settings );
    return;
    break;

  case INPUT_KEY_Return:
  case INPUT_KEY_KP_Enter:
  case INPUT_JOYSTICK_FIRE_1:
    widget_end_all( WIDGET_FINISHED_OK );
    widget_$_->{name}_running = 0;
    display_refresh_all();
    return;
    break;

  default:	/* Keep gcc happy */
    break;

  \}

  if( cursor_pressed ) \{
    int old_highlight_line = highlight_line;
    highlight_line = new_highlight_line;
    options_$_->{name}\[old_highlight_line+1\].draw( menu_left_edge_x, menu_width, options_$_->{name} + old_highlight_line + 1, &widget_options_settings );
    options_$_->{name}\[highlight_line+1\].draw( menu_left_edge_x, menu_width, options_$_->{name} + highlight_line + 1, &widget_options_settings );
    return;
  \}

  for( ptr=&options_$_->{name}\[1\]; ptr->text != NULL; ptr++ ) \{
    if( key == ptr->key ) \{
      int old_highlight_line = highlight_line;
      ptr->click();
      highlight_line = ptr->index;
      options_$_->{name}\[old_highlight_line+1\].draw( menu_left_edge_x, menu_width, options_$_->{name} + old_highlight_line + 1, &widget_options_settings );
      ptr->draw( menu_left_edge_x, menu_width, ptr, &widget_options_settings );
      break;
    \}
  \}
\}
CODE

}

print "\n#endif				/* #ifdef USE_WIDGET */\n";

