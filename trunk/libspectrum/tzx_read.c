/* tzx_read.c: Routines for reading .tzx files
   Copyright (c) 2001, 2002 Philip Kendall, Darren Salt

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

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "internals.h"

/* The .tzx file signature (first 8 bytes) */
static const libspectrum_byte *signature = "ZXTape!\x1a";

/*** Local function prototypes ***/

static libspectrum_error
tzx_read_rom_block( libspectrum_tape *tape, const libspectrum_byte **ptr,
		    const libspectrum_byte *end );
static libspectrum_error
tzx_read_turbo_block( libspectrum_tape *tape, const libspectrum_byte **ptr,
		      const libspectrum_byte *end );
static libspectrum_error
tzx_read_pure_tone( libspectrum_tape *tape, const libspectrum_byte **ptr,
		    const libspectrum_byte *end );
static libspectrum_error
tzx_read_pulses_block( libspectrum_tape *tape, const libspectrum_byte **ptr,
		       const libspectrum_byte *end );
static libspectrum_error
tzx_read_pure_data( libspectrum_tape *tape, const libspectrum_byte **ptr,
		    const libspectrum_byte *end );
static libspectrum_error
tzx_read_raw_data( libspectrum_tape *tape, const libspectrum_byte **ptr,
		   const libspectrum_byte *end );
static libspectrum_error
tzx_read_pause( libspectrum_tape *tape, const libspectrum_byte **ptr,
		const libspectrum_byte *end );
static libspectrum_error
tzx_read_group_start( libspectrum_tape *tape, const libspectrum_byte **ptr,
		      const libspectrum_byte *end );
static libspectrum_error
tzx_read_jump( libspectrum_tape *tape, const libspectrum_byte **ptr,
	       const libspectrum_byte *end );
static libspectrum_error
tzx_read_loop_start( libspectrum_tape *tape, const libspectrum_byte **ptr,
		     const libspectrum_byte *end );
static libspectrum_error
tzx_read_select( libspectrum_tape *tape, const libspectrum_byte **ptr,
		 const libspectrum_byte *end );
static libspectrum_error
tzx_read_stop( libspectrum_tape *tape, const libspectrum_byte **ptr,
	       const libspectrum_byte *end );
static libspectrum_error
tzx_read_comment( libspectrum_tape *tape, const libspectrum_byte **ptr,
		  const libspectrum_byte *end );
static libspectrum_error
tzx_read_message( libspectrum_tape *tape, const libspectrum_byte **ptr,
		  const libspectrum_byte *end );
static libspectrum_error
tzx_read_archive_info( libspectrum_tape *tape, const libspectrum_byte **ptr,
		       const libspectrum_byte *end );
static libspectrum_error
tzx_read_hardware( libspectrum_tape *tape, const libspectrum_byte **ptr,
		   const libspectrum_byte *end );
static libspectrum_error
tzx_read_custom( libspectrum_tape *tape, const libspectrum_byte **ptr,
		 const libspectrum_byte *end );
static libspectrum_error
tzx_read_concat( const libspectrum_byte **ptr, const libspectrum_byte *end );

static libspectrum_error
tzx_read_empty_block( libspectrum_tape *tape, libspectrum_tape_type id );

static libspectrum_error
tzx_read_data( const libspectrum_byte **ptr, const libspectrum_byte *end,
	       size_t *length, int bytes, libspectrum_byte **data );
static libspectrum_error
tzx_read_string( const libspectrum_byte **ptr, const libspectrum_byte *end,
		 libspectrum_byte **dest );

/*** Function definitions ***/

/* The main load function */

libspectrum_error
libspectrum_tzx_read( libspectrum_tape *tape, const libspectrum_byte *buffer,
		      const size_t length )
{

  libspectrum_error error;

  const libspectrum_byte *ptr, *end;

  ptr = buffer; end = buffer + length;

  /* Must be at least as many bytes as the signature, and the major/minor
     version numbers */
  if( length < strlen(signature) + 2 ) {
    libspectrum_print_error(
      LIBSPECTRUM_ERROR_CORRUPT,
      "libspectrum_tzx_create: not enough data in buffer"
    );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  /* Now check the signature */
  if( memcmp( ptr, signature, strlen( signature ) ) ) {
    libspectrum_print_error( LIBSPECTRUM_ERROR_SIGNATURE,
			     "libspectrum_tzx_create: wrong signature" );
    return LIBSPECTRUM_ERROR_SIGNATURE;
  }
  ptr += strlen( signature );
  
  /* Just skip the version numbers */
  ptr += 2;

  while( ptr < end ) {

    /* Get the ID of the next block */
    libspectrum_tape_type id = *ptr++;

    switch( id ) {
    case LIBSPECTRUM_TAPE_BLOCK_ROM:
      error = tzx_read_rom_block( tape, &ptr, end );
       if( error ) { libspectrum_tape_free( tape ); return error; }
      break;
    case LIBSPECTRUM_TAPE_BLOCK_TURBO:
      error = tzx_read_turbo_block( tape, &ptr, end );
      if( error ) { libspectrum_tape_free( tape ); return error; }
      break;
    case LIBSPECTRUM_TAPE_BLOCK_PURE_TONE:
      error = tzx_read_pure_tone( tape, &ptr, end );
      if( error ) { libspectrum_tape_free( tape ); return error; }
      break;
    case LIBSPECTRUM_TAPE_BLOCK_PULSES:
      error = tzx_read_pulses_block( tape, &ptr, end );
      if( error ) { libspectrum_tape_free( tape ); return error; }
      break;
    case LIBSPECTRUM_TAPE_BLOCK_PURE_DATA:
      error = tzx_read_pure_data( tape, &ptr, end );
      if( error ) { libspectrum_tape_free( tape ); return error; }
      break;
    case LIBSPECTRUM_TAPE_BLOCK_RAW_DATA:
      error = tzx_read_raw_data( tape, &ptr, end );
      if( error ) { libspectrum_tape_free( tape ); return error; }
      break;

    case LIBSPECTRUM_TAPE_BLOCK_PAUSE:
      error = tzx_read_pause( tape, &ptr, end );
      if( error ) { libspectrum_tape_free( tape ); return error; }
      break;
    case LIBSPECTRUM_TAPE_BLOCK_GROUP_START:
      error = tzx_read_group_start( tape, &ptr, end );
      if( error ) { libspectrum_tape_free( tape ); return error; }
      break;
    case LIBSPECTRUM_TAPE_BLOCK_GROUP_END:
      error = tzx_read_empty_block( tape, id );
      if( error ) { libspectrum_tape_free( tape ); return error; }
      break;
    case LIBSPECTRUM_TAPE_BLOCK_JUMP:
      error = tzx_read_jump( tape, &ptr, end );
      if( error ) { libspectrum_tape_free( tape ); return error; }
      break;
    case LIBSPECTRUM_TAPE_BLOCK_LOOP_START:
      error = tzx_read_loop_start( tape, &ptr, end );
      if( error ) { libspectrum_tape_free( tape ); return error; }
      break;
    case LIBSPECTRUM_TAPE_BLOCK_LOOP_END:
      error = tzx_read_empty_block( tape, id );
      if( error ) { libspectrum_tape_free( tape ); return error; }
      break;

    case LIBSPECTRUM_TAPE_BLOCK_SELECT:
      error = tzx_read_select( tape, &ptr, end );
      if( error ) { libspectrum_tape_free( tape ); return error; }
      break;

    case LIBSPECTRUM_TAPE_BLOCK_STOP48:
      error = tzx_read_stop( tape, &ptr, end );
      if( error ) { libspectrum_tape_free( tape ); return error; }
      break;

    case LIBSPECTRUM_TAPE_BLOCK_COMMENT:
      error = tzx_read_comment( tape, &ptr, end );
      if( error ) { libspectrum_tape_free( tape ); return error; }
      break;
    case LIBSPECTRUM_TAPE_BLOCK_MESSAGE:
      error = tzx_read_message( tape, &ptr, end );
      if( error ) { libspectrum_tape_free( tape ); return error; }
      break;
    case LIBSPECTRUM_TAPE_BLOCK_ARCHIVE_INFO:
      error = tzx_read_archive_info( tape, &ptr, end );
      if( error ) { libspectrum_tape_free( tape ); return error; }
      break;
    case LIBSPECTRUM_TAPE_BLOCK_HARDWARE:
      error = tzx_read_hardware( tape, &ptr, end );
      if( error ) { libspectrum_tape_free( tape ); return error; }
      break;

    case LIBSPECTRUM_TAPE_BLOCK_CUSTOM:
      error = tzx_read_custom( tape, &ptr, end );
      if( error ) { libspectrum_tape_free( tape ); return error; }
      break;

    case LIBSPECTRUM_TAPE_BLOCK_CONCAT:
      error = tzx_read_concat( &ptr, end );
      if( error ) { libspectrum_tape_free( tape ); return error; }
      break;

    default:	/* For now, don't handle anything else */
      libspectrum_tape_free( tape );
      libspectrum_print_error(
        LIBSPECTRUM_ERROR_UNKNOWN,
        "libspectrum_tzx_create: unknown block type 0x%02x", id
      );
      return LIBSPECTRUM_ERROR_UNKNOWN;
    }
  }

  return LIBSPECTRUM_ERROR_NONE;
}

static libspectrum_error
tzx_read_rom_block( libspectrum_tape *tape, const libspectrum_byte **ptr,
		    const libspectrum_byte *end )
{
  libspectrum_tape_block* block;
  size_t length; libspectrum_byte *data;
  libspectrum_error error;

  /* Check there's enough left in the buffer for the pause and the
     data length */
  if( end - (*ptr) < 4 ) {
    libspectrum_print_error( LIBSPECTRUM_ERROR_CORRUPT,
			     "tzx_read_rom_block: not enough data in buffer" );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  /* Get a new block */
  error = libspectrum_tape_block_alloc( &block, LIBSPECTRUM_TAPE_BLOCK_ROM );
  if( error ) return error;

  /* Get the pause length */
  libspectrum_tape_block_set_pause( block, (*ptr)[0] + (*ptr)[1] * 0x100 );
  (*ptr) += 2;

  /* And the data */
  error = tzx_read_data( ptr, end, &length, 2, &data );
  if( error ) { free( block ); return error; }
  libspectrum_tape_block_set_data_length( block, length );
  libspectrum_tape_block_set_data( block, data );

  /* Finally, put the block into the block list */
  error = libspectrum_tape_append_block( tape, block );
  if( error ) { libspectrum_tape_block_free( block ); return error; }

  /* And return with no error */
  return LIBSPECTRUM_ERROR_NONE;

}

static libspectrum_error
tzx_read_turbo_block( libspectrum_tape *tape, const libspectrum_byte **ptr,
		      const libspectrum_byte *end )
{
  libspectrum_tape_block* block;
  size_t length; libspectrum_byte *data;
  libspectrum_error error;

  /* Check there's enough left in the buffer for all the metadata */
  if( end - (*ptr) < 18 ) {
    libspectrum_print_error(
      LIBSPECTRUM_ERROR_CORRUPT,
      "tzx_read_turbo_block: not enough data in buffer"
    );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  /* Get a new block */
  error = libspectrum_tape_block_alloc( &block, LIBSPECTRUM_TAPE_BLOCK_TURBO );
  if( error ) return error;

  /* Get the metadata */
  libspectrum_tape_block_set_pilot_length( block,
					   (*ptr)[0] + (*ptr)[1] * 0x100 );
  (*ptr) += 2;
  libspectrum_tape_block_set_sync1_length( block,
					   (*ptr)[0] + (*ptr)[1] * 0x100 );
  (*ptr) += 2;
  libspectrum_tape_block_set_sync2_length( block,
					   (*ptr)[0] + (*ptr)[1] * 0x100 );
  (*ptr) += 2;
  libspectrum_tape_block_set_bit0_length ( block,
					   (*ptr)[0] + (*ptr)[1] * 0x100 );
  (*ptr) += 2;
  libspectrum_tape_block_set_bit1_length ( block,
					   (*ptr)[0] + (*ptr)[1] * 0x100 );
  (*ptr) += 2;
  libspectrum_tape_block_set_pilot_pulses( block,
					   (*ptr)[0] + (*ptr)[1] * 0x100 );
  (*ptr) += 2;
  libspectrum_tape_block_set_bits_in_last_byte( block, **ptr ); (*ptr)++;
  libspectrum_tape_block_set_pause       ( block,
					   (*ptr)[0] + (*ptr)[1] * 0x100 );
  (*ptr) += 2;

  /* Read the data in */
  error = tzx_read_data( ptr, end, &length, 3, &data );
  if( error ) { free( block ); return error; }
  libspectrum_tape_block_set_data_length( block, length );
  libspectrum_tape_block_set_data( block, data );

  /* Finally, put the block into the block list */
  error = libspectrum_tape_append_block( tape, block );
  if( error ) { libspectrum_tape_block_free( block ); return error; }

  /* And return with no error */
  return LIBSPECTRUM_ERROR_NONE;

}

static libspectrum_error
tzx_read_pure_tone( libspectrum_tape *tape, const libspectrum_byte **ptr,
		    const libspectrum_byte *end )
{
  libspectrum_tape_block* block;
  libspectrum_error error;

  /* Check we've got enough bytes */
  if( end - (*ptr) < 4 ) {
    libspectrum_print_error( LIBSPECTRUM_ERROR_CORRUPT,
			     "tzx_read_pure_tone: not enough data in buffer" );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  /* Get a new block */
  error = libspectrum_tape_block_alloc( &block,
					LIBSPECTRUM_TAPE_BLOCK_PURE_TONE );
  if( error ) return error;

  /* Read in the data, and move along */
  libspectrum_tape_block_set_pulse_length( block,
					   (*ptr)[0] + (*ptr)[1] * 0x100 );
  (*ptr) += 2;
  libspectrum_tape_block_set_count( block, (*ptr)[0] + (*ptr)[1] * 0x100 );
  (*ptr) += 2;
  
  /* Finally, put the block into the block list */
  error = libspectrum_tape_append_block( tape, block );
  if( error ) { libspectrum_tape_block_free( block ); return error; }

  /* And return with no error */
  return LIBSPECTRUM_ERROR_NONE;
}

static libspectrum_error
tzx_read_pulses_block( libspectrum_tape *tape, const libspectrum_byte **ptr,
		       const libspectrum_byte *end )
{
  libspectrum_tape_block* block;
  libspectrum_dword *lengths;
  libspectrum_error error; size_t i, count;

  /* Check the count byte exists */
  if( (*ptr) == end ) {
    libspectrum_print_error(
      LIBSPECTRUM_ERROR_CORRUPT,
      "tzx_read_pulses_block: not enough data in buffer"
    );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  /* Get a new block */
  error = libspectrum_tape_block_alloc( &block,
					LIBSPECTRUM_TAPE_BLOCK_PULSES );
  if( error ) return error;

  /* Get the count byte */
  count = **ptr; (*ptr)++;
  libspectrum_tape_block_set_count( block, count );

  /* Check enough data exists for every pulse */
  if( end - (*ptr) < 2 * count ) {
    free( block );
    libspectrum_print_error(
      LIBSPECTRUM_ERROR_CORRUPT,
      "tzx_read_pulses_block: not enough data in buffer"
    );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  /* Allocate memory for the actual list of lengths */
  lengths = malloc( count * sizeof( libspectrum_dword ) );
  if( lengths == NULL ) {
    free( block );
    libspectrum_print_error( LIBSPECTRUM_ERROR_MEMORY,
			     "tzx_read_pulses_block: out of memory" );
    return LIBSPECTRUM_ERROR_MEMORY;
  }

  /* Copy the data across */
  for( i = 0; i < count; i++ ) {
    lengths[i] = (*ptr)[0] + (*ptr)[1] * 0x100; (*ptr) += 2;
  }
  libspectrum_tape_block_set_pulse_lengths( block, lengths );

  /* Put the block into the block list */
  error = libspectrum_tape_append_block( tape, block );
  if( error ) { libspectrum_tape_block_free( block ); return error; }

  /* And return with no error */
  return LIBSPECTRUM_ERROR_NONE;
}

static libspectrum_error
tzx_read_pure_data( libspectrum_tape *tape, const libspectrum_byte **ptr,
		    const libspectrum_byte *end )
{
  libspectrum_tape_block* block;
  size_t length; libspectrum_byte *data;

  libspectrum_error error;

  /* Check there's enough left in the buffer for all the metadata */
  if( end - (*ptr) < 10 ) {
    libspectrum_print_error(
      LIBSPECTRUM_ERROR_CORRUPT,
      "tzx_read_pure_data: not enough data in buffer"
    );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  /* Get a new block */
  error = libspectrum_tape_block_alloc( &block,
					LIBSPECTRUM_TAPE_BLOCK_PURE_DATA );
  if( error ) return error;

  /* Get the metadata */
  libspectrum_tape_block_set_bit0_length( block,
					  (*ptr)[0] + (*ptr)[1] * 0x100 );
  (*ptr) += 2;
  libspectrum_tape_block_set_bit1_length( block,
					  (*ptr)[0] + (*ptr)[1] * 0x100 );
  (*ptr) += 2;
  libspectrum_tape_block_set_bits_in_last_byte( block, **ptr ); (*ptr)++;
  libspectrum_tape_block_set_pause( block, (*ptr)[0] + (*ptr)[1] * 0x100 );
  (*ptr) += 2;

  /* And the actual data */
  error = tzx_read_data( ptr, end, &length, 3, &data );
  if( error ) { free( block ); return error; }
  libspectrum_tape_block_set_data_length( block, length );
  libspectrum_tape_block_set_data( block, data );

  /* Finally, put the block into the block list */
  error = libspectrum_tape_append_block( tape, block );
  if( error ) { libspectrum_tape_block_free( block ); return error; }

  /* And return with no error */
  return LIBSPECTRUM_ERROR_NONE;

}

static libspectrum_error
tzx_read_raw_data (libspectrum_tape *tape, const libspectrum_byte **ptr,
		   const libspectrum_byte *end)
{
  libspectrum_tape_block* block;
  size_t length; libspectrum_byte *data;
  libspectrum_error error;

  /* Check there's enough left in the buffer for all the metadata */
  if (end - (*ptr) < 8) {
    libspectrum_print_error( LIBSPECTRUM_ERROR_CORRUPT,
			     "tzx_read_raw_data: not enough data in buffer" );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  /* Get a new block */
  error = libspectrum_tape_block_alloc( &block,
					LIBSPECTRUM_TAPE_BLOCK_RAW_DATA );
  if( error ) return error;

  /* Get the metadata */
  libspectrum_tape_block_set_bit_length( block,
					 (*ptr)[0] + (*ptr)[1] * 0x100 );
  libspectrum_tape_block_set_pause     ( block,
					 (*ptr)[2] + (*ptr)[3] * 0x100 );
  libspectrum_tape_block_set_bits_in_last_byte( block, (*ptr)[4] );
  (*ptr) += 5;

  /* And the actual data */
  error = tzx_read_data( ptr, end, &length, 3, &data );
  if( error ) { free( block ); return error; }
  libspectrum_tape_block_set_data_length( block, length );
  libspectrum_tape_block_set_data( block, data );

  /* Finally, put the block into the block list */
  error = libspectrum_tape_append_block( tape, block );
  if( error ) { libspectrum_tape_block_free( block ); return error; }

  /* And return with no error */
  return LIBSPECTRUM_ERROR_NONE;
}

static libspectrum_error
tzx_read_pause( libspectrum_tape *tape, const libspectrum_byte **ptr,
		const libspectrum_byte *end )
{
  libspectrum_tape_block *block;
  libspectrum_error error;

  /* Check the pause actually exists */
  if( end - (*ptr) < 2 ) {
    libspectrum_print_error( LIBSPECTRUM_ERROR_CORRUPT,
			     "tzx_read_pause: not enough data in buffer" );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  /* Get a new block */
  error = libspectrum_tape_block_alloc( &block,
					LIBSPECTRUM_TAPE_BLOCK_PAUSE );
  if( error ) return error;

  /* Get the pause length */
  libspectrum_tape_block_set_pause( block, (*ptr)[0] + (*ptr)[1] * 0x100 );
  (*ptr) += 2;

  /* Put the block into the block list */
  error = libspectrum_tape_append_block( tape, block );
  if( error ) { libspectrum_tape_block_free( block ); return error; }

  /* And return */
  return LIBSPECTRUM_ERROR_NONE;
}
  
static libspectrum_error
tzx_read_group_start( libspectrum_tape *tape, const libspectrum_byte **ptr,
		      const libspectrum_byte *end )
{
  libspectrum_tape_block *block;
  libspectrum_byte *name;
  libspectrum_error error;
  
  /* Check the length byte exists */
  if( (*ptr) == end ) {
    libspectrum_print_error(
      LIBSPECTRUM_ERROR_CORRUPT,
      "tzx_read_group_start: not enough data in buffer"
    );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  /* Get a new block */
  error = libspectrum_tape_block_alloc( &block,
					LIBSPECTRUM_TAPE_BLOCK_GROUP_START );
  if( error ) return error;

  /* Read in the description of the group */
  error = tzx_read_string( ptr, end, &name );
  if( error ) { free( block ); return error; }
  libspectrum_tape_block_set_text( block, name );
			  
  /* Finally, put the block into the block list */
  error = libspectrum_tape_append_block( tape, block );
  if( error ) { libspectrum_tape_block_free( block ); return error; }

  return LIBSPECTRUM_ERROR_NONE;
}

static libspectrum_error
tzx_read_jump( libspectrum_tape *tape, const libspectrum_byte **ptr,
	       const libspectrum_byte *end )
{
  libspectrum_tape_block *block;
  int offset;
  libspectrum_error error;
  
  /* Check the offset exists */
  if( end - (*ptr) < 2 ) {
    libspectrum_print_error( LIBSPECTRUM_ERROR_CORRUPT,
			     "tzx_read_jump: not enough data in buffer" );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  /* Get a new block */
  error = libspectrum_tape_block_alloc( &block, LIBSPECTRUM_TAPE_BLOCK_JUMP );
  if( error ) return error;

  /* Get the offset */
  offset = (*ptr)[0] + (*ptr)[1] * 0x100; (*ptr) += 2;
  if( offset >= 32768 ) offset -= 65536;
  libspectrum_tape_block_set_offset( block, offset);

  /* Finally, put the block into the block list */
  error = libspectrum_tape_append_block( tape, block );
  if( error ) { libspectrum_tape_block_free( block ); return error; }

  return LIBSPECTRUM_ERROR_NONE;
}

static libspectrum_error
tzx_read_loop_start( libspectrum_tape *tape, const libspectrum_byte **ptr,
		     const libspectrum_byte *end )
{
  libspectrum_tape_block *block;
  libspectrum_error error;
  
  /* Check the count exists */
  if( end - (*ptr) < 2 ) {
    libspectrum_print_error(
      LIBSPECTRUM_ERROR_CORRUPT,
      "tzx_read_loop_start: not enough data in buffer"
    );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  /* Get a new block */
  error = libspectrum_tape_block_alloc( &block,
					LIBSPECTRUM_TAPE_BLOCK_LOOP_START );
  if( error ) return error;

  /* Get the repeat count */
  libspectrum_tape_block_set_count( block, (*ptr)[0] + (*ptr)[1] * 0x100 );
  (*ptr) += 2;

  /* Finally, put the block into the block list */
  error = libspectrum_tape_append_block( tape, block );
  if( error ) { libspectrum_tape_block_free( block ); return error; }

  return LIBSPECTRUM_ERROR_NONE;
}

static libspectrum_error
tzx_read_select( libspectrum_tape *tape, const libspectrum_byte **ptr,
		 const libspectrum_byte *end )
{
  libspectrum_tape_block* block;
  libspectrum_error error;

  size_t length, count, i, j;
  int *offsets;
  libspectrum_byte **descriptions;

  /* Check there's enough left in the buffer for the length and the count
     byte */
  if( end - (*ptr) < 3 ) {
    libspectrum_print_error( LIBSPECTRUM_ERROR_CORRUPT,
			     "tzx_read_select: not enough data in buffer" );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  /* Get the length, and check we've got that many bytes in the buffer */
  length = (*ptr)[0] + (*ptr)[1] * 0x100; (*ptr) += 2;
  if( end - (*ptr) < (ptrdiff_t)length ) {
    libspectrum_print_error( LIBSPECTRUM_ERROR_CORRUPT,
			     "tzx_read_select: not enough data in buffer" );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  /* Get a new block */
  error = libspectrum_tape_block_alloc( &block,
					LIBSPECTRUM_TAPE_BLOCK_SELECT );
  if( error ) return error;

  /* Get the number of selections */
  count = **ptr; (*ptr)++;
  libspectrum_tape_block_set_count( block, count ); (*ptr)++;

  /* Allocate memory */
  offsets = malloc( count * sizeof( int ) );
  if( !offsets ) {
    free( block );
    libspectrum_print_error( LIBSPECTRUM_ERROR_MEMORY, 
			     "tzx_read_select: out of memory" );
    return LIBSPECTRUM_ERROR_MEMORY;
  }
  libspectrum_tape_block_set_offsets( block, offsets );

  descriptions = malloc( count * sizeof( libspectrum_byte* ) );
  if( !descriptions ) {
    free( offsets ); free( block );
    libspectrum_print_error( LIBSPECTRUM_ERROR_MEMORY,
			     "tzx_read_select: out of memory" );
    return LIBSPECTRUM_ERROR_MEMORY;
  }
  libspectrum_tape_block_set_texts( block, descriptions );

  /* Read in the data */
  for( i = 0; i < count; i++ ) {

    /* Check we've got the offset and a length byte */
    if( end - (*ptr) < 3 ) {
      for( j = 0; j < i; j++ ) free( descriptions[j] );
      free( descriptions ); free( offsets ); free( block );
      libspectrum_print_error( LIBSPECTRUM_ERROR_CORRUPT,
			       "tzx_read_select: not enough data in buffer" );
      return LIBSPECTRUM_ERROR_CORRUPT;
    }

    /* Get the offset */
    offsets[i] = (*ptr)[0] + (*ptr)[1] * 0x100; (*ptr) += 2;

    /* Get the description of this selection */
    error = tzx_read_string( ptr, end, &descriptions[i] );
    if( error ) {
      for( j = 0; j < i; j++ ) free( descriptions[j] );
      free( descriptions ); free( offsets ); free( block );
      return error;
    }

  }

  /* Finally, put the block into the block list */
  error = libspectrum_tape_append_block( tape, block );
  if( error ) { libspectrum_tape_block_free( block ); return error; }

  return LIBSPECTRUM_ERROR_NONE;
}

static libspectrum_error
tzx_read_stop( libspectrum_tape *tape, const libspectrum_byte **ptr,
	       const libspectrum_byte *end )
{
  libspectrum_tape_block *block;
  libspectrum_error error;

  /* Check the length field exists */
  if( end - (*ptr) < 4 ) {
    libspectrum_print_error( LIBSPECTRUM_ERROR_CORRUPT,
			     "tzx_read_stop: not enough data in buffer" );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  /* But then just skip over it, as I don't care what it is */
  (*ptr) += 4;

  /* Get a new block */
  error = libspectrum_tape_block_alloc( &block,
					LIBSPECTRUM_TAPE_BLOCK_STOP48 );
  if( error ) return error;

  /* Put the block into the block list */
  error = libspectrum_tape_append_block( tape, block );
  if( error ) { libspectrum_tape_block_free( block ); return error; }

  return LIBSPECTRUM_ERROR_NONE;
}  

static libspectrum_error
tzx_read_comment( libspectrum_tape *tape, const libspectrum_byte **ptr,
		  const libspectrum_byte *end )
{
  libspectrum_tape_block* block;
  libspectrum_byte *text;
  libspectrum_error error;

  /* Check the length byte exists */
  if( (*ptr) == end ) {
    libspectrum_print_error( LIBSPECTRUM_ERROR_CORRUPT,
			     "tzx_read_comment: not enough data in buffer" );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  /* Get a new block */
  error = libspectrum_tape_block_alloc( &block,
					LIBSPECTRUM_TAPE_BLOCK_COMMENT );
  if( error ) return error;

  /* Get the actual comment */
  error = tzx_read_string( ptr, end, &text );
  if( error ) { free( block ); return error; }
  libspectrum_tape_block_set_text( block, text );

  /* Finally, put the block into the block list */
  error = libspectrum_tape_append_block( tape, block );
  if( error ) { libspectrum_tape_block_free( block ); return error; }

  return LIBSPECTRUM_ERROR_NONE;
}

static libspectrum_error
tzx_read_message( libspectrum_tape *tape, const libspectrum_byte **ptr,
		  const libspectrum_byte *end )
{
  libspectrum_tape_block* block;
  libspectrum_byte *text;
  libspectrum_error error;

  /* Check the time and length byte exists */
  if( end - (*ptr) < 2 ) {
    libspectrum_print_error( LIBSPECTRUM_ERROR_CORRUPT,
			     "tzx_read_message: not enough data in buffer" );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  /* Get a new block */
  error = libspectrum_tape_block_alloc( &block,
					LIBSPECTRUM_TAPE_BLOCK_MESSAGE );
  if( error ) return error;

  /* Get the time */
  libspectrum_tape_block_set_pause( block, **ptr ); (*ptr)++;

  /* Get the message itself */
  error = tzx_read_string( ptr, end, &text );
  if( error ) { free( block ); return error; }
  libspectrum_tape_block_set_text( block, text );

  /* Finally, put the block into the block list */
  error = libspectrum_tape_append_block( tape, block );
  if( error ) { libspectrum_tape_block_free( block ); return error; }

  return LIBSPECTRUM_ERROR_NONE;
}

static libspectrum_error
tzx_read_archive_info( libspectrum_tape *tape, const libspectrum_byte **ptr,
		       const libspectrum_byte *end )
{
  libspectrum_tape_block* block;
  libspectrum_error error;

  size_t i, count;
  int *ids;
  libspectrum_byte **strings;

  /* Check there's enough left in the buffer for the length and the count
     byte */
  if( end - (*ptr) < 3 ) {
    libspectrum_print_error(
      LIBSPECTRUM_ERROR_CORRUPT,
      "tzx_read_archive_info: not enough data in buffer"
    );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  /* Get a new block */
  error = libspectrum_tape_block_alloc( &block,
					LIBSPECTRUM_TAPE_BLOCK_ARCHIVE_INFO );
  if( error ) return error;

  /* Skip the length, as I don't care about it */
  (*ptr) += 2;

  /* Get the number of string */
  count = **ptr; (*ptr)++;
  libspectrum_tape_block_set_count( block, count );

  /* Allocate memory */
  ids = malloc( count * sizeof( int ) );
  if( !ids ) {
    free( block );
    libspectrum_print_error( LIBSPECTRUM_ERROR_MEMORY,
			     "tzx_read_archive_info: out of memory" );
    return LIBSPECTRUM_ERROR_MEMORY;
  }
  libspectrum_tape_block_set_ids( block, ids );

  strings = malloc( count * sizeof( libspectrum_byte* ) );
  if( !strings ) {
    free( ids ); free( block );
    libspectrum_print_error( LIBSPECTRUM_ERROR_MEMORY,
			     "tzx_read_archive_info: out of memory" );
    return LIBSPECTRUM_ERROR_MEMORY;
  }
  libspectrum_tape_block_set_texts( block, strings );

  for( i = 0; i < count; i++ ) {

    /* Must be ID byte and length byte */
    if( end - (*ptr) < 2 ) {
      size_t j;
      for( j=0; j<i; j++ ) free( strings[i] );
      free( strings ); free( ids ); free( block );
      libspectrum_print_error(
        LIBSPECTRUM_ERROR_CORRUPT,
        "tzx_read_archive_info: not enough data in buffer"
      );
      return LIBSPECTRUM_ERROR_CORRUPT;
    }

    /* Get the ID byte */
    ids[i] = **ptr; (*ptr)++;

    /* Read in the string itself */
    error = tzx_read_string( ptr, end, &strings[i] );
    if( error ) {
      size_t j;
      for( j = 0; j < i; j++ ) free( strings[i] );
      free( strings ); free( ids ); free( block );
      return error;
    }

  }

  /* Finally, put the block into the block list */
  error = libspectrum_tape_append_block( tape, block );
  if( error ) { libspectrum_tape_block_free( block ); return error; }

  return LIBSPECTRUM_ERROR_NONE;
}

static libspectrum_error
tzx_read_hardware( libspectrum_tape *tape, const libspectrum_byte **ptr,
		   const libspectrum_byte *end )
{
  libspectrum_tape_block* block;
  size_t i, count;
  int *types, *ids, *values;
  libspectrum_error error;

  /* Check there's enough left in the buffer for the count byte */
  if( (*ptr) == end ) {
    libspectrum_print_error( LIBSPECTRUM_ERROR_CORRUPT,
			     "tzx_read_hardware: not enough data in buffer" );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  /* Get a new block */
  error = libspectrum_tape_block_alloc( &block,
					LIBSPECTRUM_TAPE_BLOCK_HARDWARE );
  if( error ) return error;

  /* Get the number of string */
  count = **ptr; (*ptr)++;
  libspectrum_tape_block_set_count( block, count );

  /* Check there's enough data in the buffer for all the data */
  if( end - (*ptr) < 3 * (ptrdiff_t)count ) {
    libspectrum_print_error( LIBSPECTRUM_ERROR_CORRUPT,
			     "tzx_read_hardware: not enough data in buffer" );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  /* Allocate memory */
  types = malloc( count * sizeof( int ) );
  if( !types ) {
    free( block );
    libspectrum_print_error( LIBSPECTRUM_ERROR_MEMORY,
			     "tzx_read_hardware: out of memory" );
    return LIBSPECTRUM_ERROR_MEMORY;
  }
  libspectrum_tape_block_set_types( block, types );

  ids = malloc( count * sizeof( int ) );
  if( !ids ) {
    free( types ); free( block );
    libspectrum_print_error( LIBSPECTRUM_ERROR_MEMORY,
			     "tzx_read_hardware: out of memory" );
    return LIBSPECTRUM_ERROR_MEMORY;
  }
  libspectrum_tape_block_set_ids( block, ids );

  values = malloc( count * sizeof( int ) );
  if( !values ) {
    free( ids ); free( types ); free( block );
    libspectrum_print_error( LIBSPECTRUM_ERROR_MEMORY,
			     "tzx_read_hardware: out of memory" );
    return LIBSPECTRUM_ERROR_MEMORY;
  }
  libspectrum_tape_block_set_values( block, values );

  /* Actually read in all the data */
  for( i = 0; i < count; i++ ) {
    types[i]  = **ptr; (*ptr)++;
    ids[i]    = **ptr; (*ptr)++;
    values[i] = **ptr; (*ptr)++;
  }

  /* Finally, put the block into the block list */
  error = libspectrum_tape_append_block( tape, block );
  if( error ) { libspectrum_tape_block_free( block ); return error; }

  return LIBSPECTRUM_ERROR_NONE;
}

static libspectrum_error
tzx_read_custom( libspectrum_tape *tape, const libspectrum_byte **ptr,
		 const libspectrum_byte *end )
{
  libspectrum_tape_block* block;
  libspectrum_byte *description, *data; size_t length;
  libspectrum_error error;

  /* Check the description (16) and length bytes (4) exist */
  if( end - (*ptr) < 20 ) {
    libspectrum_print_error( LIBSPECTRUM_ERROR_CORRUPT,
			     "tzx_read_custom: not enough data in buffer" );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  /* Get a new block */
  error = libspectrum_tape_block_alloc( &block,
					LIBSPECTRUM_TAPE_BLOCK_CUSTOM );
  if( error ) return error;

  /* Get the description */
  description = calloc( 17, sizeof( libspectrum_byte ) );
  memcpy( description, *ptr, 16 ); (*ptr) += 16; description[16] = '\0';
  libspectrum_tape_block_set_text( block, description );

  /* Read in the data */
  error = tzx_read_data( ptr, end, &length, 4, &data );
  if( error ) { free( block ); return error; }
  libspectrum_tape_block_set_data_length( block, length );
  libspectrum_tape_block_set_data( block, data );

  /* Finally, put the block into the block list */
  error = libspectrum_tape_append_block( tape, block );
  if( error ) { libspectrum_tape_block_free( block ); return error; }

  return LIBSPECTRUM_ERROR_NONE;
}

/* Concatenation block: just skip it entirely as it serves no useful
   purpose */
static libspectrum_error
tzx_read_concat( const libspectrum_byte **ptr, const libspectrum_byte *end )
{
  /* Check there's enough data left; the -1 here is because we've already
     read the first byte of the signature as the block ID */
  if( end - (*ptr) < (ptrdiff_t)strlen( signature ) + 2 - 1 ) {
    libspectrum_print_error( LIBSPECTRUM_ERROR_CORRUPT,
			     "tzx_read_concat: not enough data in buffer" );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  /* Skip the data */
  (*ptr) += strlen( signature ) + 2 - 1;

  return LIBSPECTRUM_ERROR_NONE;
}
  
static libspectrum_error
tzx_read_empty_block( libspectrum_tape *tape, libspectrum_tape_type id )
{
  libspectrum_tape_block *block;
  libspectrum_error error;

  /* Get memory for a new block */
  error = libspectrum_tape_block_alloc( &block, id ); if( error ) return error;

  /* Put the block into the block list */
  error = libspectrum_tape_append_block( tape, block );
  if( error ) { libspectrum_tape_block_free( block ); return error; }

  return LIBSPECTRUM_ERROR_NONE;
}  

static libspectrum_error
tzx_read_data( const libspectrum_byte **ptr, const libspectrum_byte *end,
	       size_t *length, int bytes, libspectrum_byte **data )
{
  int i; libspectrum_dword multiplier = 0x01;
  size_t padding;

  if( bytes < 0 ) {
    bytes = -bytes; padding = 1;
  } else {
    padding = 0;
  }

  (*length) = 0;
  for( i=0; i<bytes; i++, multiplier *= 0x100 ) {
    *length += **ptr * multiplier; (*ptr)++;
  }

  /* Have we got enough bytes left in buffer? */
  if( ( end - (*ptr) ) < (ptrdiff_t)(*length) ) {
    libspectrum_print_error( LIBSPECTRUM_ERROR_CORRUPT,
			     "tzx_read_data: not enough data in buffer" );
    return LIBSPECTRUM_ERROR_CORRUPT;
  }

  /* Allocate memory for the data */
  (*data) =
    (libspectrum_byte*)malloc( (*length+padding) * sizeof(libspectrum_byte) );
  if( (*data) == NULL ) {
    libspectrum_print_error( LIBSPECTRUM_ERROR_MEMORY,
			     "tzx_read_data: out of memory" );
    return LIBSPECTRUM_ERROR_MEMORY;
  }

  /* Copy the block data across, and move along */
  memcpy( (*data), (*ptr), (*length) ); (*ptr) += (*length);

  return LIBSPECTRUM_ERROR_NONE;

}

static libspectrum_error
tzx_read_string( const libspectrum_byte **ptr, const libspectrum_byte *end,
		 libspectrum_byte **dest )
{
  size_t length;
  libspectrum_error error;
  libspectrum_byte *ptr2;

  error = tzx_read_data( ptr, end, &length, -1, dest );
  if( error ) return error;
  
  /* Null terminate the string */
  (*dest)[length] = '\0';

  /* Translate line endings */
  for( ptr2 = (*dest); *ptr2; ptr2++ ) if( *ptr2 == '\r' ) *ptr2 = '\n';

  return LIBSPECTRUM_ERROR_NONE;
}
