#!/usr/bin/perl -w

# accessor.pl: generate accessor functions
# Copyright (c) 2003-2004 Philip Kendall

# $Id$

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

print << "CODE";
/* snap_accessors.c: simple accessor functions for libspectrum_snap
   Copyright (c) 2003-2004 Philip Kendall

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
   Foundation, Inc., 49 Temple Place, Suite 330, Boston, MA 02111-1307 USA

   Author contact information:

   E-mail: pak21-fuse\@srcf.ucam.org
   Postal address: 15 Crescent Road, Wokingham, Berks, RG40 2DB, England

*/

/* NB: this file is autogenerated from snap_accessors.txt by accessor.pl */

#include <config.h>

#include "internals.h"

struct libspectrum_snap {

  /* Which machine are we using here? */

  libspectrum_machine machine;

  /* Registers and the like */

  libspectrum_byte a , f ; libspectrum_word bc , de , hl ;
  libspectrum_byte a_, f_; libspectrum_word bc_, de_, hl_;

  libspectrum_word ix, iy; libspectrum_byte i, r;
  libspectrum_word sp, pc;

  libspectrum_byte iff1, iff2, im;

  int halted;			/* Is the Z80 currently HALTed? */
  int last_instruction_ei;	/* Was the last instruction an EI? */

  /* RAM */

  libspectrum_byte *pages[ SNAPSHOT_RAM_PAGES ];

  /* Data from .slt files */

  libspectrum_byte *slt[ SNAPSHOT_SLT_PAGES ];	/* Level data */
  size_t slt_length[ SNAPSHOT_SLT_PAGES ];	/* Length of each level */

  libspectrum_byte *slt_screen;	/* Loading screen */
  int slt_screen_level;		/* The id of the loading screen. Not used
				   for anything AFAIK, but I\'ll copy it
				   around just in case */

  /* Peripheral status */

  libspectrum_byte out_ula; libspectrum_dword tstates;

  libspectrum_byte out_128_memoryport;

  libspectrum_byte out_ay_registerport, ay_registers[16];

  /* Used for both the +3\'s and the Scorpion\'s 0x1ffd port */
  libspectrum_byte out_plus3_memoryport;

  /* Timex-specific bits */
  libspectrum_byte out_scld_hsr, out_scld_dec;

  /* Betadisk status */
  int beta_paged;
  int beta_direction;	/* FDC seek direction:
			      zero => towards lower cylinders (hubwards)
			  non-zero => towards higher cylinders (rimwards) */
  libspectrum_byte beta_system, beta_track, beta_sector, beta_data,
    beta_status;

  /* ZXCF status */
  int zxcf_active;
  int zxcf_upload;
  libspectrum_byte zxcf_memctl;
  size_t zxcf_pages;
  libspectrum_byte *zxcf_ram[ SNAPSHOT_ZXCF_PAGES ];

  /* Interface II cartridge */
  int interface2_active;
  libspectrum_byte *interface2_rom[1];

  /* Timex Dock cartridge */
  int dock_active;
  libspectrum_byte exrom_ram[ SNAPSHOT_DOCK_EXROM_PAGES ];
  libspectrum_byte *exrom_cart[ SNAPSHOT_DOCK_EXROM_PAGES ];
  libspectrum_byte dock_ram[ SNAPSHOT_DOCK_EXROM_PAGES ];
  libspectrum_byte *dock_cart[ SNAPSHOT_DOCK_EXROM_PAGES ];

};

/* Initialise a libspectrum_snap structure */
libspectrum_error
libspectrum_snap_alloc_internal( libspectrum_snap **snap )
{
  (*snap) = malloc( sizeof( libspectrum_snap ) );
  if( !(*snap) ) {
    libspectrum_print_error( LIBSPECTRUM_ERROR_MEMORY,
			     "libspectrum_snap_alloc: out of memory" );
    return LIBSPECTRUM_ERROR_MEMORY;
  }

  return LIBSPECTRUM_ERROR_NONE;
}
CODE

while(<>) {

    next if /^\s*$/;
    next if /^\s*#/;

    my( $type, $name, $indexed ) = split;

    if( $indexed ) {

	print << "CODE";

$type
libspectrum_snap_$name( libspectrum_snap *snap, int idx )
{
  return snap->$name\[idx\];
}

void
libspectrum_snap_set_$name( libspectrum_snap *snap, int idx, $type $name )
{
  snap->$name\[idx\] = $name;
}
CODE

    } else {

	print << "CODE";

$type
libspectrum_snap_$name( libspectrum_snap *snap )
{
  return snap->$name;
}

void
libspectrum_snap_set_$name( libspectrum_snap *snap, $type $name )
{
  snap->$name = $name;
}
CODE

    }
}
