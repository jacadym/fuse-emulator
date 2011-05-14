/* tzxlist.c: Produce a listing of the blocks in a .tzx file
   Copyright (c) 2001-2011 Philip Kendall, Darren Salt, Fredrick Meunier

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

#include <errno.h>
#include <fcntl.h>
#ifdef HAVE_ICONV
#include <iconv.h>
#endif /* #ifdef HAVE_ICONV */
#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif /* #ifdef HAVE_LOCALE_H */
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <libspectrum.h>

#include "utils.h"

#define DESCRIPTION_LENGTH 80

const char *progname;

#ifdef HAVE_ICONV
static iconv_t conversion_descriptor;
#endif /* #ifdef HAVE_ICONV */

static void
dump_symbol_table( libspectrum_tape_generalised_data_symbol_table *table )
{
  size_t i, j;

  libspectrum_byte pulses = 
    libspectrum_tape_generalised_data_symbol_table_max_pulses( table );
  libspectrum_word symbols =
    libspectrum_tape_generalised_data_symbol_table_symbols_in_table( table );

  printf("    Symbols in block: %ld\n", (unsigned long)libspectrum_tape_generalised_data_symbol_table_symbols_in_block( table ) );
  printf("    Max pulses: %d\n", pulses );
  printf("    Symbols in table: %d\n", symbols );

  for( i = 0; i < symbols; i++ ) {

    libspectrum_tape_generalised_data_symbol *symbol =
      libspectrum_tape_generalised_data_symbol_table_symbol( table, i );

    printf("      Symbol %lu: type %d; pulse lengths: ", (unsigned long)i, libspectrum_tape_generalised_data_symbol_type( symbol ) );

    for( j = 0; j < pulses; j++ )
      printf( "%d ", libspectrum_tape_generalised_data_symbol_pulse( symbol, j ) );

    printf( "\n" );

  }
    
}

static void
dump_pilot_repeats( libspectrum_tape_block *block )
{
  size_t i;

  libspectrum_tape_generalised_data_symbol_table *pilot =
    libspectrum_tape_block_pilot_table( block );

  for( i = 0; i < libspectrum_tape_generalised_data_symbol_table_symbols_in_block( pilot ); i++ ) {
    printf( "    Repeat %2lu: symbol %d repeated %d times\n", (unsigned long)i,
	    libspectrum_tape_block_pilot_symbols( block, i ),
	    libspectrum_tape_block_pilot_repeats( block, i ) );
  }
}

static const char*
hardware_desc( int type, int id )
{
  switch( type ) {
  case 0:
    switch( id ) {
    case 0: return "16K Spectrum";
    case 1: return "48K Spectrum/Spectrum +";
    case 2: return "48K Spectrum (Issue 1)";
    case 3: return "128K Spectrum";
    case 4: return "Spectrum +2";
    case 5: return "Spectrum +2A/+3";
    default: return "Unknown machine";
    }
  case 3:
    switch( id ) {
    case 0: return "AY-3-8192";
    default: return "Unknown sound device";
    }
  case 4:
    switch( id ) {
    case 0: return "Kempston joystick";
    case 1: return "Cursor/Protek/AGF joystick";
    case 2: return "Sinclair joystick (Left)";
    case 3: return "Sinclair joystick (Right)";
    case 4: return "Fuller joystick";
    default: return "Unknown joystick";
    }
  default: return "Unknown type";
  }
}

/* A simple routine to convert the input text to another character encoding.
   We really are only thinking about simple conversions of accented characters,
   the pound and Euro signs so just reserve 3x the input buffer size to give
   "plenty" of space for the most common encondings and let any failure in
   conversion fall back to just returning the source string unmodified */
static char *
convert_charset( char *input )
{
#ifdef HAVE_ICONV
  const int expansion_factor = 3;
  static char *output = NULL;
  static size_t output_size = 0;
  char* output_ptr = output;
  size_t output_left = output_size;
  ICONV_CONST char* input_ptr = input;
  size_t input_left = strlen( input ) + 1;
  size_t min_output_size = input_left * expansion_factor;
 
  if( conversion_descriptor == (iconv_t)(-1) ) return input;

  if( output_size == 0 || output_size < min_output_size ) {
    output = realloc( output, min_output_size );
    if( output == NULL ) {
      output_size = 0;
      return input;
    }
    output_ptr = output;
    output_size = input_left * expansion_factor;
    output_left = output_size;
    *output = '\0';
  } 

  if( (size_t)(-1) == iconv( conversion_descriptor, &input_ptr, &input_left,
                             &output_ptr, &output_left ) ) {
    return input;
  }

  /* Write the byte sequence to get into the initial state if required. */
  if( (size_t)(-1) == iconv( conversion_descriptor, NULL, NULL, &output_ptr,
                             &output_left ) ) {
    return input;
  }

  return output;
#else  /* #ifdef HAVE_ICONV */
  return input;
#endif /* #ifdef HAVE_ICONV */
}

static void
check_checksum(unsigned long length, libspectrum_byte * data)
{
  int retval = 0;
  libspectrum_byte checksum = 0;
  if( length ) {
    size_t i;
    for( i = 0; i < length-1; i++ ) {
      checksum ^= data[i];
    }
    retval = checksum == data[length-1];
  }

  printf("  Checksum: %s\n", (retval ? "pass" : "fail"));
}

static void
print_block_name( libspectrum_byte * data )
{
  int i;
  for( i = 2; i < 12; i++) {
    printf("%c", data[i]);
  }
}

static void
decode_header( libspectrum_tape_block *block )
{
  unsigned long length = libspectrum_tape_block_data_length( block );
  libspectrum_byte * data = libspectrum_tape_block_data( block );
  int is_header = length == 19 && data[0] == 0x00;
  if(is_header) {
    switch( data[1] ) {
    case 0:
      printf("  Program: ");
      print_block_name( data );
      break;
    case 1:
      printf("  Number Array: ");
      print_block_name( data );
      break;
    case 2:
      printf("  Character Array: ");
      print_block_name( data );
      break;
    case 3:
      printf("  CODE: ");
      print_block_name( data );
      break;
    default:
      printf("  Unknown");
      break;
    }
    printf("\n");
  }
  check_checksum(length, data);
}

static int
process_tape( char *filename )
{
  int error;

  unsigned char *buffer; size_t length;
  libspectrum_tape *tape;
  libspectrum_tape_iterator iterator;
  libspectrum_tape_block *block;
  libspectrum_dword total_length = 0;
  libspectrum_id_t type;

  size_t i;

  error = read_file( filename, &buffer, &length ); if( error ) return error;

  error = libspectrum_identify_file( &type, filename, buffer, length );
  if( error != LIBSPECTRUM_ERROR_NONE ) {
    free( buffer );
    return error;
  }

#ifdef HAVE_ICONV
  /* Looks like GNU libc iconv needs to have the character type locale set
     to do the current locale target thing (but oddly GNU libiconv doesn't) */
#ifdef HAVE_SETLOCALE
  setlocale(LC_CTYPE, "");
#endif /* #ifdef HAVE_SETLOCALE */
  switch( type ) {
  case LIBSPECTRUM_ID_TAPE_TZX:
    /* Convert from Windows code page 1252 to our current locale.
       CP1252 is a valid name of the Windows-1252 charset used by WoS, Tapir and
       ZX-Blockeditor on at least Linux, MacOS X and Solaris 9. If we need to
       use another string on another OS we will need to do a little more work */
    conversion_descriptor = iconv_open("", "CP1252");
    break;
  case LIBSPECTRUM_ID_TAPE_PZX:
    conversion_descriptor = iconv_open("", "UTF-8");
    break;
  default:
    conversion_descriptor = (iconv_t)(-1);
    break;
  }
#endif /* #ifdef HAVE_ICONV */

  tape = libspectrum_tape_alloc();

  error = libspectrum_tape_read( tape, buffer, length, LIBSPECTRUM_ID_UNKNOWN,
                                 filename );
  if( error != LIBSPECTRUM_ERROR_NONE ) {
    free( buffer );
    return error;
  }

  free( buffer );

  printf("Listing of `%s':\n\n", filename );

  block = libspectrum_tape_iterator_init( &iterator, tape );

  while( block ) {
    char description[ DESCRIPTION_LENGTH ];

    error =
      libspectrum_tape_block_description( description, DESCRIPTION_LENGTH,
					  block );
    if( error ) return 1;
    printf( "Block type 0x%02x (%s)\n", libspectrum_tape_block_type( block ),
	    description );
    printf("  Block duration: %.2f sec\n",
           libspectrum_tape_block_length( block )/3500000.0 );
    total_length += libspectrum_tape_block_length( block );

    switch( libspectrum_tape_block_type( block ) ) {

    case LIBSPECTRUM_TAPE_BLOCK_ROM:
      {
        printf("  Data length: %ld bytes\n",
               (unsigned long)libspectrum_tape_block_data_length( block ) );
        decode_header( block );
        printf("  Pause length: %d ms\n",
               libspectrum_tape_block_pause( block ) );
      }
      break;

    case LIBSPECTRUM_TAPE_BLOCK_TURBO:
      printf("  %ld pilot pulses of %d tstates\n",
	     (unsigned long)libspectrum_tape_block_pilot_pulses( block ),
	     libspectrum_tape_block_pilot_length( block ) );
      printf("  Sync pulses of %d and %d tstates\n",
	     libspectrum_tape_block_sync1_length( block ),
	     libspectrum_tape_block_sync2_length( block ) );
      /* Fall through */

    case LIBSPECTRUM_TAPE_BLOCK_PURE_DATA:
      printf("  Data bits are %d (reset) and %d (set) tstates\n",
	     libspectrum_tape_block_bit0_length( block ),
	     libspectrum_tape_block_bit1_length( block ) );
      printf("  Data length: %ld bytes (%ld bits in last byte used)\n",
	     (unsigned long)libspectrum_tape_block_data_length( block ),
	     libspectrum_tape_block_bits_in_last_byte(block) ? 
               (unsigned long)libspectrum_tape_block_bits_in_last_byte(block) :
               8 );
      printf("  Pause length: %d ms\n",
	     libspectrum_tape_block_pause( block ) );
      break;

    case LIBSPECTRUM_TAPE_BLOCK_PURE_TONE:
      printf("  %ld pulses of %ld tstates\n",
	     (unsigned long)libspectrum_tape_block_count( block ),
	     (unsigned long)libspectrum_tape_block_pulse_length( block ) );
      break;

    case LIBSPECTRUM_TAPE_BLOCK_PULSES:
      for( i=0; i < libspectrum_tape_block_count( block ); i++ )
	printf("  Pulse %3ld: length %d tstates\n",
	       (unsigned long)i,
	       libspectrum_tape_block_pulse_lengths( block, i ) );
      break;

    case LIBSPECTRUM_TAPE_BLOCK_RAW_DATA:
      printf("  Length: %ld bytes\n", (unsigned long)
	     libspectrum_tape_block_data_length( block ) );
      printf("  Bits in last byte: %ld\n",
	     (unsigned long)libspectrum_tape_block_bits_in_last_byte(block) );
      printf("  Each bit is %d tstates\n",
	     libspectrum_tape_block_bit_length( block ) );
      printf("  Pause length: %d ms\n",
	     libspectrum_tape_block_pause( block ) );
      break;

    case LIBSPECTRUM_TAPE_BLOCK_GENERALISED_DATA:
      printf("  Pause length: %d ms\n", libspectrum_tape_block_pause( block ) );
      printf("  Pilot table:\n");
      dump_symbol_table( libspectrum_tape_block_pilot_table( block ) );
      dump_pilot_repeats( block );
      printf("  Data table:\n");
      dump_symbol_table( libspectrum_tape_block_data_table( block ) );
      break;

    case LIBSPECTRUM_TAPE_BLOCK_PAUSE:
      printf( "  Initial polarity: " );
      if( libspectrum_tape_block_level( block ) < 0 ) {
        printf( "none specified\n" );
      } else {
        printf( "%s\n",
                libspectrum_tape_block_level( block ) ? "high" : "low" );
      }
      printf("  Length: %d ms\n", libspectrum_tape_block_pause( block ) );
      break;

    case LIBSPECTRUM_TAPE_BLOCK_GROUP_START:
      printf("  Name: %s\n", 
                    convert_charset( libspectrum_tape_block_text( block ) ) );
      break;

    case LIBSPECTRUM_TAPE_BLOCK_GROUP_END:
    case LIBSPECTRUM_TAPE_BLOCK_LOOP_END:
    case LIBSPECTRUM_TAPE_BLOCK_STOP48:
      /* Do nothing */
      break;

    case LIBSPECTRUM_TAPE_BLOCK_SET_SIGNAL_LEVEL:
      printf( "  Polarity: %s\n",
                libspectrum_tape_block_level( block ) ? "high" : "low" );
      break;

    case LIBSPECTRUM_TAPE_BLOCK_JUMP:
      printf("  Offset: %d\n", libspectrum_tape_block_offset( block ) );
      break;

    case LIBSPECTRUM_TAPE_BLOCK_LOOP_START:
      printf("  Count: %lu\n",
	     (unsigned long)libspectrum_tape_block_count( block ) );
      break;

    case LIBSPECTRUM_TAPE_BLOCK_SELECT:
      for( i = 0; i < libspectrum_tape_block_count( block ); i++ ) {
	printf("  Choice %2ld: Offset %d: %s\n", (unsigned long)i,
	       libspectrum_tape_block_offsets( block, i ),
               convert_charset( libspectrum_tape_block_texts( block, i ) ) );
      }
      break;


    case LIBSPECTRUM_TAPE_BLOCK_MESSAGE:
      printf("  Display for %d seconds\n",
	     libspectrum_tape_block_pause( block ) );
      /* Fall through */

    case LIBSPECTRUM_TAPE_BLOCK_COMMENT:
      printf("  Comment: %s\n", 
               convert_charset( libspectrum_tape_block_text( block ) ) );
      break;

    case LIBSPECTRUM_TAPE_BLOCK_ARCHIVE_INFO:
      for( i = 0; i < libspectrum_tape_block_count( block ); i++ ) {
	printf("  ");
	switch( libspectrum_tape_block_ids( block, i ) ) {
	case   0: printf("Full Title:"); break;
	case   1: printf(" Publisher:"); break;
	case   2: printf("    Author:"); break;
	case   3: printf("      Year:"); break;
	case   4: printf("  Language:"); break;
	case   5: printf("  Category:"); break;
	case   6: printf("     Price:"); break;
	case   7: printf("    Loader:"); break;
	case   8: printf("    Origin:"); break;
	case 255: printf("   Comment:"); break;
        default: printf("(Unknown string): "); break;
	}
	printf(" %s\n", 
               convert_charset( libspectrum_tape_block_texts( block, i ) ));
      }
      break;

    case LIBSPECTRUM_TAPE_BLOCK_HARDWARE:
      for( i = 0; i < libspectrum_tape_block_count( block ); i++ ) {
	printf( "  %s: ",
	        hardware_desc( libspectrum_tape_block_types( block, i ),
			       libspectrum_tape_block_ids( block, i )
			     )
	      );
	switch( libspectrum_tape_block_values( block, i ) ) {
	case 0: printf("runs"); break;
	case 1: printf("runs, using hardware"); break;
	case 2: printf("runs, does not use hardware"); break;
	case 3: printf("does not run"); break;
	}
	printf("\n");
      }
      break;

    case LIBSPECTRUM_TAPE_BLOCK_CUSTOM:
      printf( "  Description: %s\n", 
                    convert_charset( libspectrum_tape_block_text( block ) ) );
      printf( "       Length: %ld bytes\n",
	      (unsigned long)libspectrum_tape_block_data_length( block ) );
      break;

    case LIBSPECTRUM_TAPE_BLOCK_PULSE_SEQUENCE:
      printf( "  Initial polarity: low\n" );
      for( i=0; i < libspectrum_tape_block_count( block ); i++ )
        printf("  Pulse %3ld: length %4d tstates, repeated %4ld times\n",
             (unsigned long)i,
             libspectrum_tape_block_pulse_lengths( block, i ),
             (unsigned long)libspectrum_tape_block_pulse_repeats( block, i ) );
      break;

    case LIBSPECTRUM_TAPE_BLOCK_DATA_BLOCK:
      printf( "  Initial polarity: " );
      if( libspectrum_tape_block_level( block ) < 0 ) {
        printf( "none specified\n" );
      } else {
        printf( "%s\n",
                libspectrum_tape_block_level( block ) ? "high" : "low" );
      }
      printf("  Reset data bits are\n");
      for( i=0; i < libspectrum_tape_block_bit0_pulse_count( block ); i++ )
	printf("    Pulse %3ld: length %4d tstates\n",
             (unsigned long)i,
             libspectrum_tape_block_bit0_pulses( block, i ) );
      printf("  Set data bits are\n");
      for( i=0; i < libspectrum_tape_block_bit1_pulse_count( block ); i++ )
	printf("    Pulse %3ld: length %4d tstates\n",
             (unsigned long)i,
             libspectrum_tape_block_bit1_pulses( block, i ) );
      printf("  Data length: %ld bytes (%ld bits in last byte used)\n",
	     (unsigned long)libspectrum_tape_block_data_length( block ),
	     libspectrum_tape_block_bits_in_last_byte(block) ? 
               (unsigned long)libspectrum_tape_block_bits_in_last_byte(block) :
               8 );
      printf("  Tail length: %d tstates\n",
	     libspectrum_tape_block_tail_length( block ) );
      break;

    default:
      printf("  (Sorry -- %s can't handle that kind of block. Skipping it)\n",
	     progname );
      break;
    }

    block = libspectrum_tape_iterator_next( &iterator );

  }

  printf( "Total tape duration: %.2f sec\n", total_length/3500000.0 );

  error = libspectrum_tape_free( tape );
  if( error != LIBSPECTRUM_ERROR_NONE ) return error;

#ifdef HAVE_ICONV
  if( conversion_descriptor != (iconv_t)(-1) ) {
    error = iconv_close( conversion_descriptor ); if( error ) return error;
  }
#endif /* #ifdef HAVE_ICONV */

  return 0;
}

int
main( int argc, char **argv )
{
  int ret = 0;
  int arg = 0;
  int error;

  progname = argv[0];

  if( argc < 2 ) {
    fprintf( stderr, "%s: usage: %s <tape files>...\n", progname, progname );
    return 1;
  }

  error = init_libspectrum(); if( error ) return error;


  while( ++arg < argc )
    ret |= process_tape( argv[arg] );

  return ret;
}
