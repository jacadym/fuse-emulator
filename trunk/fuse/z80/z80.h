/* z80.h: z80 emulation core
   Copyright (c) 1999-2003 Philip Kendall

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

#ifndef FUSE_Z80_H
#define FUSE_Z80_H

/* Union allowing a register pair to be accessed as bytes or as a word */
typedef union {
#ifdef WORDS_BIGENDIAN
  struct { libspectrum_byte h,l; } b;
#else
  struct { libspectrum_byte l,h; } b;
#endif
  libspectrum_word w;
} regpair;

/* What's stored in the main processor */
typedef struct {
  regpair af,bc,de,hl;
  regpair af_,bc_,de_,hl_;
  regpair ix,iy;
  libspectrum_byte i;
  libspectrum_word r;	/* The low seven bits of the R register. 16 bits long
			   so it can also act as an RZX instruction counter */
  libspectrum_byte r7;	/* The high bit of the R register */
  regpair sp,pc;
  libspectrum_byte iff1, iff2, im;
  int halted;
} processor;

void z80_init(void);
void z80_reset(void);

void z80_interrupt(void);
void z80_nmi( void );

void z80_do_opcodes(void);

void z80_enable_interrupts( void );

extern processor z80;
extern libspectrum_byte halfcarry_add_table[];
extern libspectrum_byte halfcarry_sub_table[];
extern libspectrum_byte overflow_add_table[];
extern libspectrum_byte overflow_sub_table[];
extern libspectrum_byte sz53_table[];
extern libspectrum_byte sz53p_table[];
extern libspectrum_byte parity_table[];

#endif			/* #ifndef FUSE_Z80_H */
