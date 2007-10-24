/* pentagon.c: Pentagon 128K specific routines
   Copyright (c) 1999-2007 Philip Kendall and Fredrick Meunier

   $Id$

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

   Author contact information:

   E-mail: philip-fuse@shadowmagic.org.uk

*/

#include <config.h>

#include <libspectrum.h>

#include "compat.h"
#include "disk/beta.h"
#include "joystick.h"
#include "machine.h"
#include "machines.h"
#include "memory.h"
#include "periph.h"
#include "settings.h"
#include "spec128.h"
#include "ula.h"

static libspectrum_byte pentagon_select_1f_read( libspectrum_word port,
						 int *attached );
static libspectrum_byte pentagon_select_ff_read( libspectrum_word port,
						 int *attached );
static libspectrum_byte pentagon_contend_delay( libspectrum_dword time );
static int pentagon_reset( void );
static int pentagon_shutdown( void );
static int pentagon_memory_map( void );

static const periph_t peripherals[] = {
  { 0x00ff, 0x001f, pentagon_select_1f_read, beta_cr_write },
  { 0x00ff, 0x003f, beta_tr_read, beta_tr_write },
  { 0x00ff, 0x005f, beta_sec_read, beta_sec_write },
  { 0x00ff, 0x007f, beta_dr_read, beta_dr_write },
  { 0x00ff, 0x00fe, ula_read, ula_write },
  { 0x00ff, 0x00ff, pentagon_select_ff_read, beta_sp_write },
  { 0xc002, 0xc000, ay_registerport_read, ay_registerport_write },
  { 0xc002, 0x8000, NULL, ay_dataport_write },
  { 0xc002, 0x4000, NULL, spec128_memoryport_write },
};

static const size_t peripherals_count =
  sizeof( peripherals ) / sizeof( periph_t );

static int pentagon_512 = 0;

static libspectrum_byte
pentagon_select_1f_read( libspectrum_word port, int *attached )
{
  libspectrum_byte data;

  data = beta_sr_read( port, attached ); if( *attached ) return data;
  data = joystick_kempston_read( port, attached ); if( *attached ) return data;

  return 0xff;
}

static libspectrum_byte
pentagon_select_ff_read( libspectrum_word port, int *attached )
{
  libspectrum_byte data;

  data = beta_sp_read( port, attached ); if( *attached ) return data;

  return 0xff;
}

static libspectrum_byte
pentagon_unattached_port( void )
{
  return spectrum_unattached_port();
}

int
pentagon_port_contended( libspectrum_word port GCC_UNUSED )
{
  /* No contended ports */
  return 0;
}

static libspectrum_byte
pentagon_contend_delay( libspectrum_dword time GCC_UNUSED )
{
  /* No contention */
  return 0;
}

int
pentagon_init( fuse_machine_info *machine )
{
  machine->machine = LIBSPECTRUM_MACHINE_PENT;
  machine->id = "pentagon";

  pentagon_512 = 0;

  machine->reset = pentagon_reset;

  machine->timex = 0;
  machine->ram.port_contended = pentagon_port_contended;
  machine->ram.contend_delay  = pentagon_contend_delay;
  machine->ram.contend_delay_no_mreq = pentagon_contend_delay;

  machine->unattached_port = pentagon_unattached_port;

  machine->shutdown = pentagon_shutdown;

  machine->memory_map = pentagon_memory_map;

  return 0;

}

int 
pentagon512_init( fuse_machine_info *machine )
{
  pentagon_init( machine );

  machine->machine = LIBSPECTRUM_MACHINE_PENT512;
  machine->id = "pentagon512";

  pentagon_512 = 1;

  return 0;
}

static int
pentagon_reset(void)
{
  int error;
  int i;

  error = machine_load_rom( 0, 0, settings_current.rom_pentagon_0,
                            settings_default.rom_pentagon_0, 0x4000 );
  if( error ) return error;
  error = machine_load_rom( 2, 1, settings_current.rom_pentagon_1,
                            settings_default.rom_pentagon_1, 0x4000 );
  if( error ) return error;
  error = machine_load_rom( 4, 2, settings_current.rom_pentagon_3,
                            settings_default.rom_pentagon_3, 0x4000 );
  if( error ) return error;
  error = machine_load_rom_bank( memory_map_romcs, 0, 0,
                                 settings_current.rom_pentagon_2,
                                 settings_default.rom_pentagon_2, 0x4000 );
  if( error ) return error;

  beta_available = 1;
  beta_active = 1;

  error = spec128_common_reset( 0 );
  if( error ) return error;

  beta_reset();

  error = periph_setup( peripherals, peripherals_count );
  if( error ) return error;
  periph_setup_kempston( PERIPH_PRESENT_OPTIONAL );
  periph_update();

  machine_current->ram.last_byte2 = 0;
  machine_current->ram.special = 0;

  if( pentagon_512 ) {
    /* Mark the least 384K as present/writeable */
    for( i = 16; i < 64; i++ )
      memory_map_ram[i].writable = 1;
  }

  return 0;
}

static int
pentagon_shutdown( void )
{
  beta_end();

  return 0;
}

static int
pentagon_memory_map( void )
{
  int rom, page, screen;
  size_t i;

  screen = ( machine_current->ram.last_byte & 0x08 ) ? 7 : 5;
  if( memory_current_screen != screen ) {
    display_update_critical( 0, 0 );
    display_refresh_main_screen();
    memory_current_screen = screen;
  }

  if( beta_active && !( machine_current->ram.last_byte & 0x10 ) ) {
    rom = 2;
  } else {
    rom = ( machine_current->ram.last_byte & 0x10 ) >> 4;
  }

  machine_current->ram.current_rom = rom;

  spec128_select_rom( rom );

  page = machine_current->ram.last_byte & 0x07;

  if( pentagon_512 ) {
    page += ( machine_current->ram.last_byte & 0xC0 ) >> 3;
  }

  spec128_select_page( page );
  machine_current->ram.current_page = page;

  for( i = 0; i < 8; i++ )
    memory_map_read[i] = memory_map_write[i] = *memory_map_home[i];

  memory_romcs_map();

  return 0;
}
