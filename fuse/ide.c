/* ide.c: Generic routines shared between the various IDE devices
   Copyright (c) 2005 Philip Kendall
		 
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

   E-mail: Philip Kendall <pak21-fuse@srcf.ucam.org>
   Postal address: 15 Crescent Road, Wokingham, Berks, RG40 2DB, England

*/

#include <config.h>

#include <libspectrum.h>

#include "ui/ui.h"

int
ide_eject( libspectrum_ide_channel *chn, libspectrum_ide_unit unit,
	   int (*commit_fn)( libspectrum_ide_unit unit ), char **setting,
	   ui_menu_item item )
{
  int error;

  if( libspectrum_ide_dirty( chn, unit ) ) {
    
    ui_confirm_save_t confirm = ui_confirm_save(
      "Hard disk has been modified.\nDo you want to save it?"
    );
  
    switch( confirm ) {

    case UI_CONFIRM_SAVE_SAVE:
      error = commit_fn( unit ); if( error ) return error;
      break;

    case UI_CONFIRM_SAVE_DONTSAVE: break;
    case UI_CONFIRM_SAVE_CANCEL: return 0;

    }
  }

  free( *setting ); *setting = NULL;
  
  error = libspectrum_ide_eject( chn, unit );
  if( error ) return error;

  error = ui_menu_activate( item, 0 );
  if( error ) return error;

  return 0;
}
