/* statusbar.c: routines for updating the status bar
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

#ifdef UI_GTK		/* Use this file iff we're using GTK+ */

#include <stdio.h>

#include <gtk/gtk.h>

#include "gtkinternals.h"
#include "ui/ui.h"

GtkWidget *disk_status,	/* Is the disk motor running? */
  *pause_status,	/* Is emulation paused (via the menu option)? */
  *tape_status,		/* Is the tape running? */
  *speed_status;	/* How fast are we running? */

int
gtkstatusbar_create( GtkBox *parent )
{
  GtkWidget *status_bar;

  status_bar = gtk_hbox_new( FALSE, 3 );
  gtk_box_pack_start( parent, status_bar, FALSE, FALSE, 0 );

  disk_status = gtk_label_new( "Disk: N/A" );
  gtk_box_pack_start_defaults( GTK_BOX( status_bar ), disk_status );

  pause_status = gtk_label_new( "Paused: 0" );
  gtk_box_pack_start_defaults( GTK_BOX( status_bar ), pause_status );

  tape_status = gtk_label_new( "Tape: 0" );
  gtk_box_pack_start_defaults( GTK_BOX( status_bar ), tape_status );

  speed_status = gtk_label_new( "100%" );
  gtk_box_pack_start_defaults( GTK_BOX( status_bar ), speed_status );

  return 0;
}

int
ui_statusbar_update( ui_statusbar_item item, ui_statusbar_state state )
{
  switch( item ) {

  case UI_STATUSBAR_ITEM_DISK:
    switch( state ) {
    case UI_STATUSBAR_STATE_NOT_AVAILABLE:
      gtk_label_set( GTK_LABEL( disk_status ), "Disk: N/A" ); break;
    case UI_STATUSBAR_STATE_ACTIVE:
      gtk_label_set( GTK_LABEL( disk_status ), "Disk: 1" ); break;
    default:
      gtk_label_set( GTK_LABEL( disk_status ), "Disk: 0" ); break;
    }      
    return 0;

  case UI_STATUSBAR_ITEM_PAUSED:
    gtk_label_set(
      GTK_LABEL( pause_status ),
      state == UI_STATUSBAR_STATE_ACTIVE ? "Paused: 1" : "Paused: 0"
    );
    return 0;

  case UI_STATUSBAR_ITEM_TAPE:
    gtk_label_set( GTK_LABEL( tape_status ),
		   state == UI_STATUSBAR_STATE_ACTIVE ? "Tape: 1" : "Tape: 0"
		 );
    return 0;

  }

  ui_error( UI_ERROR_ERROR, "Attempt to update unknown statusbar item %d",
	    item );
  return 1;
}

int
ui_statusbar_update_speed( float speed )
{
  char buffer[8];

  snprintf( buffer, 8, "%3.0f%%", speed );
  gtk_label_set( GTK_LABEL( speed_status ), buffer );

  return 0;
}

#endif			/* #ifdef UI_GTK */
