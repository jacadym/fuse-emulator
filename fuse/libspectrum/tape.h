/* tape.h: Routines for handling tape files
   Copyright (c) 2001 Philip Kendall

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

#ifndef LIBSPECTRUM_TAPE_H
#define LIBSPECTRUM_TAPE_H

#include <config.h>

#include <glib.h>	/* FIXME: what to do if we don't have glib? */

#ifndef LIBSPECTRUM_LIBSPECTRUM_H
#include "libspectrum.h"
#endif			/* #ifndef LIBSPECTRUM_LIBSPECTRUM_H */

/* The various types of block available */
typedef enum libspectrum_tape_type {
  LIBSPECTRUM_TAPE_BLOCK_ROM,
} libspectrum_tape_type;

/* A huge number of states available; encompasses all possible block types */
typedef enum libspectrum_tape_state_type {

  /* Normal/turbo loaders */
  LIBSPECTRUM_TAPE_STATE_PILOT,		/* Pilot pulses */
  LIBSPECTRUM_TAPE_STATE_SYNC1,		/* First sync pulse */
  LIBSPECTRUM_TAPE_STATE_SYNC2,		/* Second sync pulse */
  LIBSPECTRUM_TAPE_STATE_DATA1,		/* First edge of a data bit */
  LIBSPECTRUM_TAPE_STATE_DATA2,		/* Second edge of a data bit */

  /* Generic */
  LIBSPECTRUM_TAPE_STATE_PAUSE,		/* The pause at the end of a block */

} libspectrum_tape_state_type;

/* A standard ROM loading block */
typedef struct libspectrum_tape_rom_block {
  
  size_t length;		/* How long is this block */
  libspectrum_byte *data;	/* The actual data */
  libspectrum_dword pause;	/* Pause after block (milliseconds) */

  /* Private data */

  libspectrum_tape_state_type state;

  size_t edge_count;		/* Number of pilot pulses to go */

  size_t bytes_through_block;
  size_t bits_through_byte;	/* How far through the data are we? */

  libspectrum_byte current_byte; /* The current data byte; gets shifted out
				    as we read bits from it */
  libspectrum_dword bit_tstates; /* How long is an edge for the current bit */

} libspectrum_tape_rom_block;

/* The timings for the standard ROM loader */
const libspectrum_dword LIBSPECTRUM_TAPE_TIMING_PILOT = 2168; /* Pilot */
const libspectrum_dword LIBSPECTRUM_TAPE_TIMING_SYNC1 =  667; /* Sync 1 */
const libspectrum_dword LIBSPECTRUM_TAPE_TIMING_SYNC2 =  735; /* Sync 2 */
const libspectrum_dword LIBSPECTRUM_TAPE_TIMING_DATA0 =  855; /* Reset */
const libspectrum_dword LIBSPECTRUM_TAPE_TIMING_DATA1 = 1710; /* Set */

/* A generic tape block */
typedef struct libspectrum_tape_block {

  libspectrum_tape_type type;

  union {
    libspectrum_tape_rom_block rom;
  } types;

} libspectrum_tape_block;

typedef struct libspectrum_tape {

  /* All the blocks */
  GSList* blocks;

  /* The current block */
  GSList* current_block;

} libspectrum_tape;

#endif				/* #ifndef LIBSPECTRUM_TAPE_H */
