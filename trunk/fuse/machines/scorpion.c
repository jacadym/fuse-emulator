/* scorpion.c: Scorpion 256K specific routines
   Copyright (c) 1999-2004 Philip Kendall and Fredrick Meunier
   Copyright (c) 2004 Stuart Brady

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
#include <unistd.h>

#include <libspectrum.h>

#include "ay.h"
#include "compat.h"
#include "joystick.h"
#include "machines.h"
#include "memory.h"
#include "printer.h"
#include "settings.h"
#include "scorpion.h"
#include "spec128.h"
#include "specplus3.h"
#include "spectrum.h"
#include "trdos.h"

static libspectrum_byte scorpion_select_1f_read( libspectrum_word port,
						 int *attached );
static libspectrum_byte scorpion_select_ff_read( libspectrum_word port,
						 int *attached );
static libspectrum_byte scorpion_contend_delay( libspectrum_dword time );
static int scorpion_reset( void );
static int scorpion_shutdown( void );
static int scorpion_memory_map( void );

const static periph_t peripherals[] = {
  { 0x00ff, 0x001f, scorpion_select_1f_read, trdos_cr_write },
  { 0x00ff, 0x003f, trdos_tr_read, trdos_tr_write },
  { 0x00ff, 0x005f, trdos_sec_read, trdos_sec_write },
  { 0x00ff, 0x007f, trdos_dr_read, trdos_dr_write },
  { 0x00ff, 0x00fe, spectrum_ula_read, spectrum_ula_write },
  { 0x00ff, 0x00ff, scorpion_select_ff_read, trdos_sp_write },
  { 0xc002, 0xc000, ay_registerport_read, ay_registerport_write },
  { 0xc002, 0x8000, NULL, ay_dataport_write },
  { 0xc002, 0x4000, NULL, spec128_memoryport_write },
  { 0xf002, 0x1000, NULL, specplus3_memoryport2_write },
};

const static size_t peripherals_count =
  sizeof( peripherals ) / sizeof( periph_t );

static libspectrum_byte
scorpion_select_1f_read( libspectrum_word port, int *attached )
{
  libspectrum_byte data;

  data = trdos_sr_read( port, attached ); if( *attached ) return data;
  data = joystick_kempston_read( port, attached ); if( *attached ) return data;

  return 0xff;
}

static libspectrum_byte
scorpion_select_ff_read( libspectrum_word port, int *attached )
{
  libspectrum_byte data;

  data = trdos_sp_read( port, attached ); if( *attached ) return data;

  return 0xff;
}

static libspectrum_byte
scorpion_unattached_port( void )
{
  return spectrum_unattached_port( 3 );
}

static libspectrum_byte
scorpion_contend_delay( libspectrum_dword time GCC_UNUSED )
{
  /* No contention */
  return 0;
}

int
scorpion_init( fuse_machine_info *machine )
{
  machine->machine = LIBSPECTRUM_MACHINE_SCORP;
  machine->id = "scorpion";

  machine->reset = scorpion_reset;

  machine->timex = 0;
  machine->ram.port_contended = pentagon_port_contended;
  machine->ram.contend_delay  = scorpion_contend_delay;

  machine->unattached_port = scorpion_unattached_port;

  machine->shutdown = scorpion_shutdown;

  machine->memory_map = scorpion_memory_map;

  return 0;
}

int
scorpion_reset(void)
{
  int error;

  trdos_reset();

  error = machine_load_rom( 0, 0, settings_current.rom_scorpion_0, 0x4000 );
  if( error ) return error;
  error = machine_load_rom( 2, 1, settings_current.rom_scorpion_1, 0x4000 );
  if( error ) return error;
  error = machine_load_rom( 4, 3, settings_current.rom_scorpion_3, 0x4000 );
  if( error ) return error;
  error = machine_load_rom( 6, 2, settings_current.rom_scorpion_2, 0x4000 );
  if( error ) return error;

  trdos_available = 1;
  trdos_active = 0;

  error = periph_setup( peripherals, peripherals_count,
			PERIPH_PRESENT_OPTIONAL,
			PERIPH_PRESENT_NEVER );
  if( error ) return error;

  machine_current->ram.last_byte2 = 0;
  machine_current->ram.special = 0;

  return spec128_common_reset( 0 );
}

static int
scorpion_memory_map( void )
{
  size_t i;
  int rom;

  int page = ( machine_current->ram.last_byte & 0x07 ) |
          ( ( machine_current->ram.last_byte2 & 0x10 ) >> 1 );
  int screen = ( machine_current->ram.last_byte & 0x08 ) ? 7 : 5;
  
  /* ROM 2 is TR-DOS because that's hardcoded elsewhere
   * ROM 3 is the extended ROM
   * 64k RAM mode takes priority over ROM 3 */
  if( ! ( machine_current->ram.last_byte2 & 0x02 ) ) {
    rom = ( machine_current->ram.last_byte & 0x10 ) >> 4;
    machine_current->ram.current_rom = rom;
    if( !( machine_current->ram.last_byte2 & 0x01 ) &&
	!trdos_active                                   )
      spec128_select_rom( rom );
  }

  spec128_select_page( page );
  
  /* If we changed the active screen, mark the entire display file as
     dirty so we redraw it on the next pass */
  if( memory_current_screen != screen ) {
    display_refresh_all();
    memory_current_screen = screen;
  }

  machine_current->ram.current_page = page;

  page = ( ( machine_current->ram.last_byte2 & 0x10 ) >> 1 ) |
          ( machine_current->ram.last_byte & 0x07 );
  
  if ( machine_current->ram.last_byte2 & 0x02 )
    rom = 3;
  else
    rom = ( machine_current->ram.last_byte & 0x10 ) >> 4;
  
  if( machine_current->ram.last_byte2 & 0x01 ) {
    /* map 0x0000-0x3fff to RAM 0 */

    memory_map_home[0] = &memory_map_ram[ 0 ];
    memory_map_home[1] = &memory_map_ram[ 1 ];
    machine_current->ram.special = 1;

    for( i = 0; i < 2; i++ )
      if( memory_map[i].bank == MEMORY_BANK_HOME )
	memory_map[i] = *memory_map_home[i];

  } else {
    spec128_select_rom( rom );
  }
  
  /* I don't think the screen can change here. Rusfaq talks about
   * 0x4000-0x7fff and 0xc000-0xffff, rather than banks 3 and 5,
   * I'm sticking with the traditional behaviour for now. */
  spec128_select_page( page );
 
  machine_current->ram.current_page = page;
  machine_current->ram.current_rom = rom;
 
  return 0;
}

static int
scorpion_shutdown( void )
{
  trdos_end();

  return 0;
}
