/* pokefinder.c: GTK+ interface to the poke finder
   Copyright (c) 2003 Philip Kendall

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

#include <gtk/gtk.h>

#include "pokefinder/pokefinder.h"

static int create_dialog( void );
static void gtkui_pokefinder_search( GtkWidget *widget, gpointer user_data );
static void gtkui_pokefinder_reset( GtkWidget *widget, gpointer user_data );
static void gtkui_pokefinder_close( GtkWidget *widget, gpointer user_data );
static void update_pokefinder( void );

static int dialog_created = 0;

static GtkWidget
  *dialog,			/* The dialog box itself */
  *count_label,			/* The number of possible locations */
  *location_list;		/* The list of possible locations */

void
gtkui_pokefinder( GtkWidget *widget, gpointer data )
{
  int error;

  if( !dialog_created ) {
    error = create_dialog(); if( error ) return;
  }

  gtk_widget_show_all( dialog );
  update_pokefinder();
}

static int
create_dialog( void )
{
  GtkWidget *hbox, *vbox, *label, *entry, *button;
  int error;
  size_t i;

  gchar *location_titles[] = { "Page", "Offset" };

  error = pokefinder_clear(); if( error ) return error;

  dialog = gtk_dialog_new();
  gtk_window_set_title( GTK_WINDOW( dialog ), "Fuse - Poke Finder" );

  hbox = gtk_hbox_new( FALSE, 0 );
  gtk_box_pack_start_defaults( GTK_BOX( GTK_DIALOG( dialog )->vbox ), hbox );

  label = gtk_label_new( "Search for:" );
  gtk_box_pack_start( GTK_BOX( hbox ), label, TRUE, TRUE, 5 );

  entry = gtk_entry_new();
  gtk_box_pack_start( GTK_BOX( hbox ), entry, TRUE, TRUE, 5 );

  vbox = gtk_vbox_new( FALSE, 0 );
  gtk_box_pack_start( GTK_BOX( hbox ), vbox, TRUE, TRUE, 5 );

  count_label = gtk_label_new( "" );
  gtk_box_pack_start( GTK_BOX( vbox ), count_label, TRUE, TRUE, 5 );

  location_list = gtk_clist_new_with_titles( 2, location_titles );
  gtk_clist_column_titles_passive( GTK_CLIST( location_list ) );
  for( i = 0; i < 2; i++ )
    gtk_clist_set_column_auto_resize( GTK_CLIST( location_list ), i, TRUE );
  gtk_box_pack_start( GTK_BOX( vbox ), location_list, TRUE, TRUE, 5 );

  button = gtk_button_new_with_label( "Search" );
  gtk_signal_connect_object( GTK_OBJECT( button ), "clicked",
		      GTK_SIGNAL_FUNC( gtkui_pokefinder_search ),
		      GTK_OBJECT( entry ) );
  gtk_container_add( GTK_CONTAINER( GTK_DIALOG( dialog )->action_area ),
		     button );

  button = gtk_button_new_with_label( "Reset" );
  gtk_signal_connect( GTK_OBJECT( button ), "clicked",
		      GTK_SIGNAL_FUNC( gtkui_pokefinder_reset ), NULL );
  gtk_container_add( GTK_CONTAINER( GTK_DIALOG( dialog )->action_area ),
		     button );

  button = gtk_button_new_with_label( "Close" );
  gtk_signal_connect_object( GTK_OBJECT( button ), "clicked",
			     GTK_SIGNAL_FUNC( gtkui_pokefinder_close ),
			     GTK_OBJECT( dialog ) );
  gtk_container_add( GTK_CONTAINER( GTK_DIALOG( dialog )->action_area ),
		     button );

  /* Users shouldn't be able to resize this window */
  gtk_window_set_policy( GTK_WINDOW( dialog ), FALSE, FALSE, TRUE );

  dialog_created = 1;

  return 0;
}

static void
gtkui_pokefinder_search( GtkWidget *widget, gpointer user_data )
{
  pokefinder_search( atoi( gtk_entry_get_text( GTK_ENTRY( widget ) ) ) );
  update_pokefinder();
}

static void
gtkui_pokefinder_reset( GtkWidget *widget, gpointer user_data )
{
  pokefinder_clear();
  update_pokefinder();
}

static void
gtkui_pokefinder_close( GtkWidget *widget, gpointer user_data )
{
  gtk_widget_hide_all( widget );
}

static void
update_pokefinder( void )
{
  size_t page, offset;
  gchar buffer[256], *possible_text[2] = { &buffer[0], &buffer[128] };

  gtk_clist_freeze( GTK_CLIST( location_list ) );
  gtk_clist_clear( GTK_CLIST( location_list ) );

  if( pokefinder_count && pokefinder_count < 20 ) {

    for( page = 0; page < 8; page++ )
      for( offset = 0; offset < 0x4000; offset++ )
	if( pokefinder_possible[page][offset] ) {
	
	  snprintf( possible_text[0], 128, "%lu", (unsigned long)page );
	  snprintf( possible_text[1], 128, "0x%04lx", (unsigned long)offset );

	  gtk_clist_append( GTK_CLIST( location_list ), possible_text );
	}

    gtk_widget_show( location_list );

  } else {
    gtk_widget_hide( location_list );
  }

  gtk_clist_thaw( GTK_CLIST( location_list ) );

  snprintf( buffer, 256, "Possible locations: %lu",
	    (unsigned long)pokefinder_count );
  gtk_label_set_text( GTK_LABEL( count_label ), buffer );
}  
