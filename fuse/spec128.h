/* spec128.h: Spectrum 128K specific routines
   Copyright (c) 1999-2001 Philip Kendall

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

#ifndef FUSE_SPEC128_H
#define FUSE_SPEC128_H

#include <libspectrum.h>

#ifndef FUSE_MACHINE_H
#include "machine.h"
#endif			/* #ifndef FUSE_MACHINE_H */

extern spectrum_port_info spec128_peripherals[];

libspectrum_byte spec128_unattached_port( void );

libspectrum_byte spec128_readbyte( libspectrum_word address );
libspectrum_byte spec128_readbyte_internal( libspectrum_word address );
libspectrum_byte spec128_read_screen_memory( libspectrum_word offset );
void spec128_writebyte( libspectrum_word address, libspectrum_byte b );
void spec128_writebyte_internal( libspectrum_word address,
				 libspectrum_byte b );

libspectrum_dword spec128_contend_memory( libspectrum_word address );
libspectrum_dword spec128_contend_port( libspectrum_word port );
libspectrum_byte spec128_contend_delay( libspectrum_dword time );

int spec128_init( fuse_machine_info *machine );
int spec128_reset(void);
int spec128_common_reset( int contention );

void spec128_memoryport_write( libspectrum_word port, libspectrum_byte b );

#endif			/* #ifndef FUSE_SPEC128_H */
