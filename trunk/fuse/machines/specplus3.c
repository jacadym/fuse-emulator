/* specplus3.c: Spectrum +2A/+3 specific routines
   Copyright (c) 1999-2000 Philip Kendall

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

   E-mail: pak@ast.cam.ac.uk
   Postal address: 15 Crescent Road, Wokingham, Berks, RG40 2DB, England

*/

#include <stdio.h>
#include <string.h>

#include "ay.h"
#include "display.h"
#include "keyboard.h"
#include "specplus3.h"
#include "spectrum.h"
#include "z80.h"

BYTE specplus3_readbyte(WORD address)
{
  int bank;

  if(machine.ram.special) {
    bank=address/0x4000; address-=(bank*0x4000);
    switch(machine.ram.specialcfg) {
      case 0: return RAM[bank  ][address];
      case 1: return RAM[bank+4][address];
      case 2: switch(bank) {
	case 0: return RAM[4][address];
	case 1: return RAM[5][address];
	case 2: return RAM[6][address];
	case 3: return RAM[3][address];
      }
      case 3: switch(bank) {
	case 0: return RAM[4][address];
	case 1: return RAM[7][address];
	case 2: return RAM[6][address];
	case 3: return RAM[3][address];
      }
      default:
	fprintf(stderr,"Unknown +3 special configuration %d\n",
		machine.ram.specialcfg);
	abort();
    }
  } else {
    if(address<0x4000) return ROM[machine.ram.current_rom][address];
    bank=address/0x4000; address-=(bank*0x4000);
    switch(bank) {
      case 1: return RAM[                       5][address]; break;
      case 2: return RAM[                       2][address]; break;
      case 3: return RAM[machine.ram.current_page][address]; break;
      default: abort();
    }
  }
}

BYTE specplus3_read_screen_memory(WORD offset)
{
  return RAM[machine.ram.current_screen][offset];
}

void specplus3_writebyte(WORD address, BYTE b)
{
  int bank;

  if(machine.ram.special) {
    bank=address/0x4000; address-=(bank*0x4000);
    switch(machine.ram.specialcfg) {
      case 0: break;
      case 1: bank+=4; break;
      case 2:
	switch(bank) {
	  case 0: bank=4; break;
	  case 1: bank=5; break;
	  case 2: bank=6; break;
	  case 3: bank=3; break;
	}
	break;
      case 3: switch(bank) {
	case 0: bank=4; break;
	case 1: bank=7; break;
	case 2: bank=6; break;
	case 3: bank=3; break;
      }
      break;
    }
  } else {
    if(address<0x4000) return;
    bank=address/0x4000; address-=(bank*0x4000);
    switch(bank) {
      case 1: bank=5;                        break;
      case 2: bank=2;                        break;
      case 3: bank=machine.ram.current_page; break;
    }
  }
  RAM[bank][address]=b;
  if(bank==machine.ram.current_screen && address < 0x1b00) {
    display_dirty(address+0x4000,b); /* Replot necessary pixels */
  }
}

int specplus3_init(void)
{
  FILE *f;

  f=fopen("plus3-0.rom","rb");
  if(!f) return 1;
  fread(ROM[0],0x4000,1,f);

  f=freopen("plus3-1.rom","rb",f);
  if(!f) return 2;
  fread(ROM[1],0x4000,1,f);

  f=freopen("plus3-2.rom","rb",f);
  if(!f) return 3;
  fread(ROM[2],0x4000,1,f);

  f=freopen("plus3-3.rom","rb",f);
  if(!f) return 4;
  fread(ROM[3],0x4000,1,f);

  fclose(f);

  readbyte=specplus3_readbyte;
  read_screen_memory=specplus3_read_screen_memory;
  writebyte=specplus3_writebyte;

  tstates=0;

  spectrum_set_timings(228,311,3.54690e6,14361);
  machine.reset=specplus3_reset;

  machine.ram.type=SPECTRUM_MACHINE_PLUS3;
  machine.ram.port=0x7ffd;
  machine.ram.port2=0x1ffd;

  machine.ay.present=1;
  machine.ay.readport=0xfffd;
  machine.ay.writeport=0xbffd;

  return 0;

}

void specplus3_reset(void)
{
  machine.ram.current_page=0; machine.ram.current_rom=0;
  machine.ram.current_screen=5;
  machine.ram.locked=0;
  machine.ram.special=0; machine.ram.specialcfg=0;
  z80_reset();
}
