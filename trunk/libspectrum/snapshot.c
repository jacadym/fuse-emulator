/* snapshot.c: Snapshot handling routines
   Copyright (c) 2001-2002 Philip Kendall, Darren Salt

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

#include <string.h>

#include "internals.h"

/* Free all memory used by a libspectrum_snap structure (destructor...) */
libspectrum_error
libspectrum_snap_free( libspectrum_snap *snap )
{
  int i;

  for( i = 0; i < 8; i++ )
    if( libspectrum_snap_pages( snap, i ) )
	free( libspectrum_snap_pages( snap, i ) );

  for( i = 0; i < 256; i++ ) {
    if( libspectrum_snap_slt_length( snap, i ) ) {
      free( libspectrum_snap_slt( snap, i ) );
      libspectrum_snap_set_slt_length( snap, i, 0 );
    }
  }
  if( libspectrum_snap_slt_screen( snap ) )
  free( libspectrum_snap_slt_screen( snap ) );

  free( snap );

  return LIBSPECTRUM_ERROR_NONE;
}

/* Read in a snapshot, optionally guessing what type it is */
libspectrum_error
libspectrum_snap_read( libspectrum_snap *snap, const libspectrum_byte *buffer,
		       size_t length, libspectrum_id_t type,
		       const char *filename )
{
  libspectrum_error error;

  /* If we don't know what sort of file this is, make a best guess */
  if( type == LIBSPECTRUM_ID_UNKNOWN ) {
    error = libspectrum_identify_file( &type, filename, buffer, length );
    if( error ) return error;

    /* If we still can't identify it, give up */
    if( type == LIBSPECTRUM_ID_UNKNOWN ) {
      libspectrum_print_error(
        LIBSPECTRUM_ERROR_UNKNOWN,
	"libspectrum_snap_read: couldn't identify file"
      );
      return LIBSPECTRUM_ERROR_UNKNOWN;
    }
  }

  switch( type ) {

  case LIBSPECTRUM_ID_SNAPSHOT_PLUSD:
    return libspectrum_plusd_read( snap, buffer, length );

  case LIBSPECTRUM_ID_SNAPSHOT_SNA:
    return libspectrum_sna_read( snap, buffer, length );

  case LIBSPECTRUM_ID_SNAPSHOT_Z80:
    return libspectrum_z80_read( snap, buffer, length );

  case LIBSPECTRUM_ID_SNAPSHOT_ZXS:
    return libspectrum_zxs_read( snap, buffer, length );

  default:
    libspectrum_print_error( LIBSPECTRUM_ERROR_CORRUPT,
			     "libspectrum_snap_read: not a snapshot file" );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }
}

/* Given a 48K memory dump `data', place it into the
   appropriate bits of `snap' for a 48K machine */
int
libspectrum_split_to_48k_pages( libspectrum_snap *snap,
				const libspectrum_byte* data )
{
  libspectrum_byte *buffer;

  /* If any of the three pages are already occupied, barf */
  if( libspectrum_snap_pages( snap, 5 ) ||
      libspectrum_snap_pages( snap, 2 ) ||
      libspectrum_snap_pages( snap, 0 )    ) {
    libspectrum_print_error(
      LIBSPECTRUM_ERROR_LOGIC,
      "libspectrum_split_to_48k_pages: RAM page already in use"
    );
    return LIBSPECTRUM_ERROR_LOGIC;
  }

  buffer = malloc( 3 * 0x4000 * sizeof( libspectrum_byte ) );
  if( !buffer ) {
    libspectrum_print_error( LIBSPECTRUM_ERROR_MEMORY,
			     "libspectrum_split_to_48k_pages: out of memory" );
    return LIBSPECTRUM_ERROR_MEMORY;
  }

  libspectrum_snap_set_pages( snap, 5, &buffer[0x0000] );
  libspectrum_snap_set_pages( snap, 2, &buffer[0x4000] );
  libspectrum_snap_set_pages( snap, 0, &buffer[0x8000] );

  /* Finally, do the copies... */
  memcpy( libspectrum_snap_pages( snap, 5 ), &data[0x0000], 0x4000 );
  memcpy( libspectrum_snap_pages( snap, 2 ), &data[0x4000], 0x4000 );
  memcpy( libspectrum_snap_pages( snap, 0 ), &data[0x8000], 0x4000 );

  return LIBSPECTRUM_ERROR_NONE;
    
}
