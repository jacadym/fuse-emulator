/* spec128.c: Spectrum 128K specific routines
   Copyright (c) 1999-2002 Philip Kendall

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

#include <libspectrum.h>

#include "ay.h"
#include "display.h"
#include "fuse.h"
#include "joystick.h"
#include "machine.h"
#include "settings.h"
#include "spec128.h"
#include "spectrum.h"

static DWORD spec128_contend_delay( void );

spectrum_port_info spec128_peripherals[] = {
  { 0x0001, 0x0000, spectrum_ula_read, spectrum_ula_write },
  { 0x00e0, 0x0000, joystick_kempston_read, spectrum_port_nowrite },
  { 0xc002, 0xc000, ay_registerport_read, ay_registerport_write },
  { 0xc002, 0x8000, spectrum_port_noread, ay_dataport_write },
  { 0x8002, 0x0000, spectrum_port_noread, spec128_memoryport_write },
  { 0, 0, NULL, NULL } /* End marker. DO NOT REMOVE */
};

BYTE spec128_unattached_port( void )
{
  return spectrum_unattached_port( 3 );
}

BYTE spec128_read_screen_memory(WORD offset)
{
  return RAM[machine_current->ram.current_screen][offset];
}

DWORD spec128_contend_memory( WORD address )
{
  /* Contention occurs in pages 1,3, 5 and 7. 0x4000 to 0x7fff is always page
     5, whilst 0xc000 to 0xffff could have one of the contended pages in */
  if( ( address >= 0x4000 && address < 0x8000 ) ||
      ( address >= 0xc000 && ( machine_current->ram.current_page & 0x01 ) )
    )
    return spec128_contend_delay();

  return 0;
}

DWORD spec128_contend_port( WORD port )
{
  /* Contention occurs for the ULA, or for the memory paging port */
  if( ( port & 0x0001 ) == 0x0000 ||
      ( port & 0xc002 ) == 0x4000    ) return spec128_contend_delay();

  return 0;
}

static DWORD spec128_contend_delay( void )
{
  WORD tstates_through_line;
  
  /* No contention in the upper border */
  if( tstates < machine_current->line_times[ DISPLAY_BORDER_HEIGHT ] )
    return 0;

  /* Or the lower border */
  if( tstates >= machine_current->line_times[ DISPLAY_BORDER_HEIGHT + 
                                              DISPLAY_HEIGHT          ] )
    return 0;

  /* Work out where we are in this line */
  tstates_through_line =
    ( tstates + machine_current->timings.left_border_cycles ) %
    machine_current->timings.cycles_per_line;

  /* No contention if we're in the left border */
  if( tstates_through_line < machine_current->timings.left_border_cycles - 3 ) 
    return 0;

  /* Or the right border or retrace */
  if( tstates_through_line >= machine_current->timings.left_border_cycles +
                              machine_current->timings.screen_cycles - 3 )
    return 0;

  /* We now know the ULA is reading the screen, so put in the appropriate
     delay */
  switch( tstates_through_line % 8 ) {
    case 5: return 6; break;
    case 6: return 5; break;
    case 7: return 4; break;
    case 0: return 3; break;
    case 1: return 2; break;
    case 2: return 1; break;
    case 3: return 0; break;
    case 4: return 0; break;
  }

  return 0;	/* Shut gcc up */
}

int spec128_init( fuse_machine_info *machine )
{
  int error;

  machine->machine = LIBSPECTRUM_MACHINE_128;
  machine->id = "128";

  machine->reset = spec128_reset;

  machine_set_timings( machine, 3.54690e6, 24, 128, 24, 52, 311, 8865);

  machine->timex = 0;
  machine->ram.read_memory	     = spec128_readbyte;
  machine->ram.read_memory_internal  = spec128_readbyte_internal;
  machine->ram.read_screen	     = spec128_read_screen_memory;
  machine->ram.write_memory          = spec128_writebyte;
  machine->ram.write_memory_internal = spec128_writebyte_internal;
  machine->ram.contend_memory	     = spec128_contend_memory;
  machine->ram.contend_port	     = spec128_contend_port;

  error = machine_allocate_roms( machine, 2 );
  if( error ) return error;
  machine->rom_length[0] = machine->rom_length[1] = 0x4000;

  machine->peripherals = spec128_peripherals;
  machine->unattached_port = spec128_unattached_port;

  machine->ay.present = 1;

  machine->shutdown = NULL;

  return 0;

}

int spec128_reset(void)
{
  int error;

  machine_current->ram.locked=0;
  machine_current->ram.current_page=0;
  machine_current->ram.current_rom=0;
  machine_current->ram.current_screen=5;

  error = machine_load_rom( &ROM[0], settings_current.rom_128_0,
			    machine_current->rom_length[0] );
  if( error ) return error;
  error = machine_load_rom( &ROM[1], settings_current.rom_128_1,
			    machine_current->rom_length[1] );
  if( error ) return error;

  return 0;
}

void
spec128_memoryport_write( WORD port GCC_UNUSED, BYTE b )
{
  int old_screen;

  /* Do nothing if we've locked the RAM configuration */
  if(machine_current->ram.locked) return;
    
  old_screen=machine_current->ram.current_screen;

  /* Store the last byte written in case we need it */
  machine_current->ram.last_byte=b;

  /* Work out which page, screen and ROM are selected */
  machine_current->ram.current_page= b & 0x07;
  machine_current->ram.current_screen=( b & 0x08 ) ? 7 : 5;
  machine_current->ram.current_rom=(machine_current->ram.current_rom & 0x02) |
    ( (b & 0x10) >> 4 );

  /* Are we locking the RAM configuration? */
  machine_current->ram.locked=( b & 0x20 );

  /* If we changed the active screen, mark the entire display file as
     dirty so we redraw it on the next pass */
  if(machine_current->ram.current_screen!=old_screen) display_refresh_all();
}
