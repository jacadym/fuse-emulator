#!/usr/bin/perl -w

# options.pl: generate options dialog boxes
# $Id$

# Copyright (c) 2002-2003 Philip Kendall

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 49 Temple Place, Suite 330, Boston, MA 02111-1307 USA

# Author contact information:

# E-mail: pak21-fuse@srcf.ucam.org
# Postal address: 15 Crescent Road, Wokingham, Berks, RG40 2DB, England

use strict;

use lib '../../perl';

use Fuse;
use Fuse::Dialog;

die "No data file specified" unless @ARGV;

my @dialogs = Fuse::Dialog::read( shift @ARGV );

print Fuse::GPL( 'options.c: options dialog boxes',
		 '2001-2002 Philip Kendall' ) . << "CODE";

/* This file is autogenerated from options.dat by options.pl.
   Do not edit unless you know what you\'re doing! */

#include <config.h>

#ifdef UI_GTK		/* Use this file iff we're using GTK+ */

#include <stdio.h>
#include <stdlib.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "compat.h"
#include "display.h"
#include "fuse.h"
#include "settings.h"
#include "gtkui.h"
CODE

foreach( @dialogs ) {

    print << "CODE";

static void gtkoptions_$_->{name}_done( GtkWidget *widget, gpointer user_data );

void
gtkoptions_$_->{name}( GtkWidget *widget GCC_UNUSED, gpointer data GCC_UNUSED )
{
  gtkoptions_$_->{name}_t dialog;
  GtkWidget *ok_button, *cancel_button, *hbox, *text, *text2;
  GtkAccelGroup *accel_group;
  gchar buffer[80];

  hbox = text = text2 = NULL;
  buffer[0] = '\\0';		/* Shut gcc up */
  
  /* Firstly, stop emulation */
  fuse_emulation_pause();

  /* Create the necessary widgets */
  dialog.dialog = gtk_dialog_new();
  gtk_window_set_title( GTK_WINDOW( dialog.dialog ), "Fuse - $_->{title}" );

  /* Create the various widgets */
CODE

    foreach my $widget ( @{ $_->{widgets} } ) {

	foreach my $type ( $widget->{type} ) {

	    my $text = $widget->{text}; $text =~ tr/()//d;

	    if( $type eq "Checkbox" ) {

		print << "CODE";
  dialog.$widget->{value} =
    gtk_check_button_new_with_label( "$text" );
  gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( dialog.$widget->{value} ),
				settings_current.$widget->{value} );
  gtk_container_add( GTK_CONTAINER( GTK_DIALOG( dialog.dialog )->vbox ),
		     dialog.$widget->{value} );

CODE
            } elsif( $type eq "Entry" ) {

                # FIXME: Make the entry widget resize sensibly

		print << "CODE";
  dialog.$widget->{value} = gtk_entry_new();
  gtk_entry_set_max_length( GTK_ENTRY( dialog.$widget->{value} ),
	   		    $widget->{data1} );
  snprintf( buffer, $widget->{data1} + 1, "%d",
	    settings_current.$widget->{value} );
  gtk_entry_set_text( GTK_ENTRY( dialog.$widget->{value} ), buffer );

  text = gtk_label_new( "$text" );
  text2 = gtk_label_new( "$widget->{data2}" );

  hbox = gtk_hbox_new( FALSE, 0 );
  gtk_box_pack_start( GTK_BOX( hbox ), text, FALSE, FALSE, 5 );
  gtk_box_pack_start_defaults( GTK_BOX( hbox ), dialog.$widget->{value} );
  gtk_box_pack_start( GTK_BOX( hbox ), text2, FALSE, FALSE, 5 );

  gtk_container_add( GTK_CONTAINER( GTK_DIALOG( dialog.dialog )->vbox ),
		     hbox );
CODE
	    } else {
		die "Unknown type `$type'";
	    }
	}
    }

    print << "CODE";
  /* Create the OK and Cancel buttons */
  ok_button = gtk_button_new_with_label( "OK" );
  cancel_button = gtk_button_new_with_label( "Cancel" );

  gtk_container_add( GTK_CONTAINER( GTK_DIALOG( dialog.dialog )->action_area ),
		     ok_button );
  gtk_container_add( GTK_CONTAINER( GTK_DIALOG( dialog.dialog )->action_area ),
		     cancel_button );

  /* Add the necessary callbacks */
  gtk_signal_connect( GTK_OBJECT( ok_button ), "clicked",
		      GTK_SIGNAL_FUNC( gtkoptions_$_->{name}_done ),
		      (gpointer) &dialog );
  gtk_signal_connect_object( GTK_OBJECT( cancel_button ), "clicked",
			     GTK_SIGNAL_FUNC( gtkui_destroy_widget_and_quit ),
			     GTK_OBJECT( dialog.dialog ) );
  gtk_signal_connect( GTK_OBJECT( dialog.dialog ), "delete_event",
		      GTK_SIGNAL_FUNC( gtkui_destroy_widget_and_quit ),
		      (gpointer) NULL );

  /* Return = 'OK', Esc = 'Cancel' */
  accel_group = gtk_accel_group_new();
  gtk_window_add_accel_group( GTK_WINDOW( dialog.dialog ), accel_group );

  gtk_widget_add_accelerator( ok_button, "clicked", accel_group,
			      GDK_Return, 0, 0 );
  gtk_widget_add_accelerator( cancel_button, "clicked", accel_group,
			      GDK_Escape, 0, 0 );

  /* Set the window to be modal and display it */
  gtk_window_set_modal( GTK_WINDOW( dialog.dialog ), TRUE );
  gtk_widget_show_all( dialog.dialog );

  /* Process events until the window is done with */
  gtk_main();

  /* And then carry on with emulation again */
  fuse_emulation_unpause();
}

static void
gtkoptions_$_->{name}_done( GtkWidget *widget GCC_UNUSED, gpointer user_data )
{
  gtkoptions_$_->{name}_t *ptr = (gtkoptions_$_->{name}_t*)user_data;

CODE

    foreach my $widget ( @{ $_->{widgets} } ) {

	if( $widget->{type} eq "Checkbox" ) {

	    print << "CODE";
  settings_current.$widget->{value} =
    gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( ptr->$widget->{value} ) );

CODE
        } elsif( $widget->{type} eq "Entry" ) {

	    print << "CODE";
  settings_current.$widget->{value} =
    atoi( gtk_entry_get_text( GTK_ENTRY( ptr->$widget->{value} ) ) );

CODE
    	} else {
	    die "Unknown type `$widget->{type}'";
	}
    }

    print << "CODE";
  gtk_widget_destroy( ptr->dialog );

  display_refresh_all();

  gtk_main_quit();
}
CODE

}

print << "CODE"

#endif			/* #ifdef UI_GTK */
CODE
