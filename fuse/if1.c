/* if2.c: Interface I handling routines
   Copyright (c) 2004 Gergely Szasz

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

   Gergely: szaszg@hu.inter.net

*/

#include <config.h>

#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include "if1.h"
#include "machine.h"
#include "memory.h"
#include "periph.h"
#include "settings.h"
#include "ui/ui.h"

#define RS232_RAW 0
#define RS232_INT 1

#define S_NET_RAW 0
#define S_NET_INT 1

#define LINE_SINCLAIR 0
#define LINE_RS232 1

/* #define IF1_DEBUG 1 */

#define CARTRIDGE_LEN 137922

typedef struct microdrive_t {
  char *filename;
  int inserted;
  int motor_on;
  int wp;
  long int head_pos;
  int transfered;
  int modified;
  long int max_bytes;
  libspectrum_byte last;
  libspectrum_byte gap;
  libspectrum_byte nogap;
  libspectrum_byte cartridge[CARTRIDGE_LEN + 1];
} microdrive_t;

typedef struct if1_ula_t {
  int fd_r;	/* file descriptor for reading bytes or bits RS232 */
  int fd_t;	/* file descriptor for writing bytes or bits RS232 */
  int fd_net;	/* file descriptor for rw bytes or bits SinclairNET */
  int rs232_mode;	/* comunication mode: RS232_RAW, RS232_INT */
  int s_net_mode;	/* comunication mode: S_NET_RAW, S_NET_INT */
  int status;	/* if1_ula/SinclairNET */
  int comms_data;	/* the previouse data comms state */
  int comms_clk;	/* the previouse data comms state */
  int cts;	/* CTS */
  int dtr;	/* DTR */
  int tx;	/* TxD */
  int rx;	/* RxD */
  int data_in;	/* interpreted incoming data */
  int count_in;
  int data_out; /* interpreted outgoing data */
  int count_out;
  
  int net_o;	/* Network out */
  int net_i;	/* Network in */
  int wait;	/* Wait state */
  int busy;	/* Indicate busy */
} if1_ula_t;

/* IF1 paged out ROM activated? */
int if1_active = 0;
int if1_available = 0;
static int if1_mdr_status = 0;

static microdrive_t microdrive[8] = {
  { .filename = NULL, .inserted = 0, .modified = 0, .wp = 0 },
  { .filename = NULL, .inserted = 0, .modified = 0, .wp = 0 },
  { .filename = NULL, .inserted = 0, .modified = 0, .wp = 0 },
  { .filename = NULL, .inserted = 0, .modified = 0, .wp = 0 },
  { .filename = NULL, .inserted = 0, .modified = 0, .wp = 0 },
  { .filename = NULL, .inserted = 0, .modified = 0, .wp = 0 },
  { .filename = NULL, .inserted = 0, .modified = 0, .wp = 0 },
  { .filename = NULL, .inserted = 0, .modified = 0, .wp = 0 }
};		/* We have 8 microdrive */

static if1_ula_t if1_ula = {
  .fd_r = -1,
  .fd_t = -1,
  .rs232_mode = RS232_INT,
  .dtr = 0,
  .comms_clk = 0,
  .comms_data = 0, /* realy? */
  .fd_net = -1,
  .s_net_mode = S_NET_INT,
};

static void microdrives_reset( void );
static void microdrives_restart( void );
static void increment_head( int m );

#define IN(m) microdrive[m - 1].inserted
#define WP(m) microdrive[m - 1].wp

enum if1_menu_item {

  UMENU_ALL = 0,
  UMENU_MDRV1,
  UMENU_MDRV2,
  UMENU_MDRV3,
  UMENU_MDRV4,
  UMENU_MDRV5,
  UMENU_MDRV6,
  UMENU_MDRV7,
  UMENU_MDRV8,
  UMENU_RS232,

};

enum if1_port {
  PORT_MDR,
  PORT_CTR,
  PORT_NET,
  PORT_UNKNOWN,
};

static void
update_menu( enum if1_menu_item what )
{
  if( what == UMENU_ALL || what == UMENU_MDRV1 ) {
    ui_menu_activate( UI_MENU_ITEM_MEDIA_IF1_M1_EJECT, IN( 1 ) );
    ui_menu_activate( UI_MENU_ITEM_MEDIA_IF1_M1_WP_SET, !WP( 1 ) );
  }

  if( what == UMENU_ALL || what == UMENU_MDRV2 ) {
    ui_menu_activate( UI_MENU_ITEM_MEDIA_IF1_M2_EJECT, IN( 2 ) );
    ui_menu_activate( UI_MENU_ITEM_MEDIA_IF1_M2_WP_SET, !WP( 2 ) );
  }
  
  if( what == UMENU_ALL || what == UMENU_MDRV3 ) {
    ui_menu_activate( UI_MENU_ITEM_MEDIA_IF1_M3_EJECT, IN( 3 ) );
    ui_menu_activate( UI_MENU_ITEM_MEDIA_IF1_M3_WP_SET, !WP( 3 ) );
  }

  if( what == UMENU_ALL || what == UMENU_MDRV4 ) {
    ui_menu_activate( UI_MENU_ITEM_MEDIA_IF1_M4_EJECT, IN( 4 ) );
    ui_menu_activate( UI_MENU_ITEM_MEDIA_IF1_M4_WP_SET, !WP( 4 ) );
  }
  
  if( what == UMENU_ALL || what == UMENU_MDRV5 ) {
    ui_menu_activate( UI_MENU_ITEM_MEDIA_IF1_M5_EJECT, IN( 5 ) );
    ui_menu_activate( UI_MENU_ITEM_MEDIA_IF1_M5_WP_SET, !WP( 5 ) );
  }
  
  if( what == UMENU_ALL || what == UMENU_MDRV6 ) {
    ui_menu_activate( UI_MENU_ITEM_MEDIA_IF1_M6_EJECT, IN( 6 ) );
    ui_menu_activate( UI_MENU_ITEM_MEDIA_IF1_M6_WP_SET, !WP( 6 ) );
  }
  
  if( what == UMENU_ALL || what == UMENU_MDRV7 ) {
    ui_menu_activate( UI_MENU_ITEM_MEDIA_IF1_M7_EJECT, IN( 7 ) );
    ui_menu_activate( UI_MENU_ITEM_MEDIA_IF1_M7_WP_SET, !WP( 7 ) );
  }
  
  if( what == UMENU_ALL || what == UMENU_MDRV8 ) {
    ui_menu_activate( UI_MENU_ITEM_MEDIA_IF1_M8_EJECT, IN( 8 ) );
    ui_menu_activate( UI_MENU_ITEM_MEDIA_IF1_M8_WP_SET, !WP( 8 ) );
  }
  
  if( what == UMENU_ALL || what == UMENU_RS232 ) {
    ui_menu_activate( UI_MENU_ITEM_MEDIA_IF1_RS232_UNPLUG_R,
                    ( if1_ula.fd_r > -1 ) ? 1 : 0 );
    ui_menu_activate( UI_MENU_ITEM_MEDIA_IF1_RS232_UNPLUG_T,
                    ( if1_ula.fd_t > -1 ) ? 1 : 0 );
    ui_menu_activate( UI_MENU_ITEM_MEDIA_IF1_SNET_UNPLUG,
                    ( if1_ula.fd_net > -1 ) ? 1 : 0 );
  }
}

void
if1_update_menu( void )
{
  update_menu( UMENU_ALL );
}

int
if1_reset( void )
{
  int error;

  if1_active = 0;
  if1_available = 0;

  if( !periph_interface1_active ) return 0;

  error = machine_load_rom_bank( memory_map_romcs, 0, 0,
				 settings_current.rom_interface_i,
				 MEMORY_PAGE_SIZE );
  if( error ) return error;

  memory_map_romcs[0].source = MEMORY_SOURCE_PERIPHERAL;

  machine_current->ram.romcs = 0;
  
  microdrives_reset();

  update_menu( UMENU_ALL );
  ui_statusbar_update( UI_STATUSBAR_ITEM_MICRODRIVE,
		       UI_STATUSBAR_STATE_INACTIVE );
  if1_mdr_status = 0;
  
  if1_available = 1;
  return 0;
}

void
if1_page( void )
{
  if1_active = 1;
  machine_current->ram.romcs = 1;
  machine_current->memory_map();
}

void
if1_unpage( void )
{
  if1_active = 0;
  machine_current->ram.romcs = 0;
  machine_current->memory_map();
}

void
if1_memory_map( void )
{
  memory_map_read[0] = memory_map_write[0] = memory_map_romcs[0];
}

static void
microdrives_reset( void )
{
  int m;

  for( m = 0; m < 8; m++ ) {
    
    microdrive[m].head_pos = 0;
    microdrive[m].motor_on = 0; /* motor off */
    microdrive[m].gap      = 15;
    microdrive[m].nogap    = 15;
/*
    microdrive[m].wp       = 0;  no write protected 
    microdrive[m].modified = 0;  not modified 
    microdrive[m].filename = NULL;
    microdrive[m].inserted = 0;  no cartridge  */
  }
  ui_statusbar_update( UI_STATUSBAR_ITEM_MICRODRIVE,
		       UI_STATUSBAR_STATE_INACTIVE );
  if1_mdr_status = 0;

  if1_ula.rs232_mode = settings_current.raw_rs232 ? RS232_RAW : RS232_INT;
  if1_ula.s_net_mode = settings_current.raw_s_net ? S_NET_RAW : S_NET_INT;
  if1_ula.comms_data = 0;
  if1_ula.count_in = 0;
  if1_ula.count_out = 0;
  if1_ula.cts = 0;
/*  if1_ula.dtr = 0; */
  if1_ula.wait = 0;
  if1_ula.busy = 0;
  if1_ula.net_o = 0;
  if1_ula.net_i = 0;

}

static enum if1_port
decode_port( libspectrum_word port )
{
    switch( port & 0x0018 ) {
    case 0x0000: return PORT_MDR;
    case 0x0008: return PORT_CTR;
    case 0x0010: return PORT_NET;
        default: return PORT_UNKNOWN;
    }
}

static libspectrum_byte
port_mdr_in( void )
{
  libspectrum_byte ret = 0xff;
  size_t i;

  for( i = 0; i < 8; i++ ) {

    microdrive_t *drive = &microdrive[ i ];

    if( drive->motor_on && drive->inserted ) {

      if( drive->transfered < drive->max_bytes ) {
	drive->last = drive->cartridge[ drive->head_pos ];
	increment_head( i );
      }

      drive->transfered++;
      ret &= drive->last;  /* I assume negative logic, but how know? */
    }

  }

  return ret;
}

static libspectrum_byte
port_ctr_in( void )
{
  libspectrum_byte ret = 0xff;
  size_t i;

  for( i = 0; i < 8; i++ ) {

    microdrive_t *drive = &microdrive[ i ];

    if( drive->motor_on && drive->inserted ) {
      if( drive->gap ) {
	/* ret &= 0xff;  GAP and SYNC high */
	drive->gap--;
      } else {
	ret &= 0xf9; /* GAP and SYNC low */
	if( drive->nogap ) {
	  drive->nogap--;
	} else {
	  drive->gap = 15;
	  drive->nogap = 15;
	}
      }
      if( drive->wp ) /* if write protected */
	ret &= 0xfe; /* active bit */
    }
  }
  /* Here we have to poll, the if1_ula 'line' dtr flag. That time it come
     from the if1_ula.fd_t status :-) */
  if( if1_ula.dtr == 0 )
    ret &= 0xf7;	/* %11110111 */

  /* Here we have to poll, the 'SinclairNet' busy flag but never used by 
     software in Interface 1 */
  if( if1_ula.busy == 0 )
    ret &= 0xef;	/* %11101111 */
  /*    fprintf( stderr, "Read CTR ( %%%d%d%d%d%d%d%d%d ).\n", 
	!!(ret & 128), !!(ret & 64), !!(ret & 32), !!(ret & 16),
	!!(ret & 8), !!(ret & 4), !!(ret & 2), !!(ret & 1)); */
  microdrives_restart();

  return ret;
}

static libspectrum_byte
port_net_in( void )
{
  libspectrum_byte ret = 0xff;

  /*  fprintf( stderr, "Read PORT NET .. " );*/
  if( if1_ula.rs232_mode == RS232_RAW ) {		/* if we do raw */
    /* Here is the input routine */
    read( if1_ula.fd_r, &if1_ula.tx, 1 );	/* Ok, if no byte, we send last*/
  } else if( if1_ula.rs232_mode == RS232_INT ) {	/* if we do interpreted */
    /* Here is the input routine */
    if( if1_ula.cts ) {	/* If CTS == 1 */
      if( if1_ula.count_in == 0 ) {
	if( if1_ula.fd_r >= 0 && read( if1_ula.fd_r, &if1_ula.data_in, 1 ) == 1 ) {
	  if1_ula.count_in++;	/* Ok, if read a byte, we begin */
	  /*	    fprintf( stderr, "Receive: %d\n", if1_ula.data_in ); */
	}
	if1_ula.tx = 0;				/* send __ later we raise :-) */
      } else if( if1_ula.count_in >= 1 && if1_ula.count_in < 5 ) {
	if1_ula.tx = 1;				/* send ~~ (start bit :-) */
	if1_ula.count_in++;
      } else if( if1_ula.count_in >= 5 && if1_ula.count_in < 13 ) {
	if1_ula.tx = ( if1_ula.data_in & 0x01 ) ? 0 : 1;
	/*				 send .. (data bits :-) */
	if1_ula.data_in >>= 1;		/* prepare next bit :-) */
	if1_ula.count_in++;
      } else 
	if1_ula.count_in = 0;
    } else {	/* if( if1_ula.cts ) */
      if1_ula.count_in = 0;		/* reset serial in */
      if1_ula.tx = 0;			/* send __ stop bits or s.e. :-) */
    }
  }
  if( if1_ula.s_net_mode == S_NET_RAW ) {		/* if we do raw */
    /* Here is the input routine */
    read( if1_ula.fd_net, &if1_ula.net_i, 1 );	/* Ok, if no byte, we send last*/
#ifdef IF1_DEBUGX
    fprintf( stderr, "Received: %d\n", if1_ula.net_o );
#endif
  } else if( if1_ula.s_net_mode == S_NET_INT ) {	/* if we do interpreted */
    /* Here is the input routine */
  }
  if( !if1_ula.tx )
    ret &= 0x7f;
  if( !if1_ula.net_i )
    ret &= 0xfe;
  microdrives_restart();

  return ret;
}

libspectrum_byte
if1_port_in( libspectrum_word port GCC_UNUSED, int *attached )
{
  libspectrum_byte ret = 0xff;

  if( !if1_active ) return ret;

  *attached = 1;

  switch( decode_port( port ) )
  {
  case PORT_MDR: ret &= port_mdr_in(); break;
  case PORT_CTR: ret &= port_ctr_in(); break;
  case PORT_NET: ret &= port_net_in(); break;
  case PORT_UNKNOWN: break;
  }

  return ret;
}

static void
port_mdr_out( libspectrum_byte val )
{
  size_t i;

  /* allow access to the port only if motor 1 is ON and there's a file open */
  for( i = 0; i < 8; i++ ) {

    microdrive_t *drive = &microdrive[ i ];

    if( drive->motor_on && drive->inserted ) {

      if( drive->transfered > 11 &&
	  drive->transfered < drive->max_bytes + 12 ) {

	drive->cartridge[ drive->head_pos ] = val;			
	increment_head( i );
	drive->modified = 1;

      }

      drive->transfered++;
    }
  }
}

static void
port_ctr_out( libspectrum_byte val )
{
  size_t m;

  if( !( val & 0x02 ) && ( if1_ula.comms_clk ) ) {	/* ~~\__ */

    for( m = 7; m > 0; m-- ) {
      /* Rotate one drive */
      microdrive[m].motor_on = microdrive[m - 1].motor_on;
    }
    microdrive[0].motor_on = (val & 0x01) ? 0 : 1;

    if( microdrive[0].motor_on || microdrive[1].motor_on || 
	microdrive[2].motor_on || microdrive[3].motor_on ||
	microdrive[4].motor_on || microdrive[5].motor_on ||
	microdrive[6].motor_on || microdrive[7].motor_on ) {
      if( !if1_mdr_status ) {
	ui_statusbar_update( UI_STATUSBAR_ITEM_MICRODRIVE,
	                     UI_STATUSBAR_STATE_ACTIVE );
	if1_mdr_status = 1;
      }
    } else if ( if1_mdr_status ) {
      ui_statusbar_update( UI_STATUSBAR_ITEM_MICRODRIVE,
			   UI_STATUSBAR_STATE_INACTIVE );
      if1_mdr_status = 0;
    }
  }
  if( val & 0x01 ) {	/* comms_data == 1 */
    /* Interface 1 service manual p.:1.4 par.: 1.5.1
       The same pin on IC1 (if1 ULA), pin 33, is used for the network
       transmit data and for the RS232 transmit data. In order to select
       the required function IC1 uses its COMMS_OUT (pin 30) signal,
       borrowed from the microdrive control when the microdrive is not
       being used (I do not know what it is exactly meaning. It is a
       hardware not being used, or a software should not use..?) This signal
       is routed from pin 30 to the emitter of transistor Q3 (RX DATA) and
       via resistor R4, to the base of transistor Q1 (NET). When COMMS_OUT
       is high Q3 is enabled this selecting RS232, and when it is low
       Q1 is enabled selecting the network.
	 
       OK, the schematics offer a different interpretation, because if
       COMMS_OUT pin level high (>+3V) then Q3 is off (the basis cannot
       be more higher potential then emitter (NPN transistor), so wathever
       is on the IC1 RX DATA (pin 33) the basis of Q4 is always on +12V,
       so the collector of Q4 (PNP transistor) always on -12V.
       If COMMS_OUT pin level goes low (~0V), then Q3 basis (connected
       to IC1 RX DATA pin) can be higher level (>+3V) than emitter, so
       the basis potential of Q4 depend on IC1 RX DATA.
	 
       OK, Summa summarum I assume that, the COMMS OUT pin is a
       negated output of the if1 ULA CTR register's COMMS DATA bit. 
    */
    /* C_DATA = 1 */
    if( if1_ula.comms_data == 0 ) {
      if1_ula.count_out = 0;
      if1_ula.data_out = 0;
      if1_ula.count_in = 0;
      if1_ula.data_in = 0;
      /*        if1_ula.status = LINE_RS232; */
    }
    /*    } else {	 ( val & 0x01 )  comms_data == 0 
	  if1_ula.status = LINE_SINCLAIR; */
  }
  if1_ula.cts = ( val & 0x10 ) ? 1 : 0;
  if1_ula.wait = ( val & 0x20 ) ? 1 : 0;
  if1_ula.comms_data = ( val & 0x01 ) ? 1 : 0;
  if1_ula.comms_clk = ( val & 0x02 ) ? 1 : 0;
    
#ifdef IF1_DEBUG
  fprintf( stderr, "Set CTS to %d, set WAIT to %d and COMMS_DATA to %d\n",
	   if1_ula.cts, if1_ula.wait, if1_ula.comms_data );
#endif

  microdrives_restart();
}

void
port_net_out( libspectrum_byte val )
{
  if( if1_ula.comms_data == 1 ) {	/* OK, if the comms_data == 1 */
    if( if1_ula.rs232_mode == RS232_INT ) {	/* if we out byte by byte, do it */
      if( if1_ula.count_out == 0 ) {	/* waiting for up edge __/~~ */
	if( if1_ula.rx == 0 && ( val & 0x01 ) == 1 ) {
	  if1_ula.count_out++;		/* get the start bit __/~~ */
	}
      } else if( if1_ula.count_out >= 1 && if1_ula.count_out < 9 ) {
	if1_ula.data_out >>= 1;
	if1_ula.data_out |= val & 0x01 ? 0 : 128;
	if1_ula.count_out++;		/* waiting for next data bit */
      } else if( if1_ula.count_out >= 9 && if1_ula.count_out < 10 ) {
	if1_ula.count_out++;		/* waiting for the 1st stop bits */
      } else if( if1_ula.count_out >= 10 ) {	/* The second stopbit */
        /* Here is the output routine */
	if( if1_ula.fd_t > -1 ) {
	  do ; while( write( if1_ula.fd_t, &if1_ula.data_out, 1 ) != 1 );
	  /*	    fprintf( stderr, "Send: %d\n", if1_ula.data_out );  */
	}
	if1_ula.count_out = 0;
	if1_ula.data_out = 0;
      }
      if1_ula.rx = val & 0x01;		/* set rx */
    } else { /*if( if1_ula.rs232_mode == RS232_RAW )  if we out bit by bit, do it */
      /* Here is the output routine for RAW communication */
      if1_ula.rx = val & 0x01;		/* set rx */
      write( if1_ula.fd_t, &if1_ula.rx, 1 );
    }
  } else {	/* if( if1_ula.comms_data == 1 ) SinclairNET :-)*/
    if( if1_ula.s_net_mode == S_NET_INT ) {	/* if we out byte by byte, do it */

      if1_ula.net_o = val & 0x01;		/* set rx */
    } else { /* if( if1_ula.s_net_mode == S_NET_RAW )  if we out bit by bit, do it */
      /* Here is the output routine */
      if1_ula.net_o = val & 0x01;		/* set rx */
      write( if1_ula.fd_net, &if1_ula.net_o, 1 );
      lseek( if1_ula.fd_net, 1, SEEK_CUR );	/* This is MY bit :-) */
#ifdef IF1_DEBUG
      fprintf( stderr, "Send SinclairNET: %d\n", if1_ula.net_o );
#endif
    }
  }
  microdrives_restart();
}

void
if1_port_out( libspectrum_word port GCC_UNUSED, libspectrum_byte val )
{
  if( !if1_active ) return;

#ifdef IF1_DEBUG
  fprintf( stderr, "In if1_port_out( %%%d%d%d%d%d%d%d%d => 0x%04x ).\n", 
	!!(val & 128), !!(val & 64), !!(val & 32), !!(val & 16),
	!!(val & 8), !!(val & 4), !!(val & 2), !!(val & 1), port);
#endif

  switch( decode_port( port ) ) {
  case PORT_MDR: port_mdr_out( val ); break;
  case PORT_CTR: port_ctr_out( val ); break;
  case PORT_NET: port_net_out( val ); break;
  case PORT_UNKNOWN: break;
  }
}

static void
increment_head( int m )
{
  microdrive[m].head_pos++;
  if( microdrive[m].head_pos >= CARTRIDGE_LEN ) microdrive[m].head_pos = 0;
}

static void
microdrives_restart( void )
{
  int m;

  for( m = 0; m < 8; m++ ) {
    while( ( microdrive[m].head_pos % 543 ) != 0  &&
           ( microdrive[m].head_pos % 543 ) != 15    )
      increment_head( m ); /* put head in the start of a block */
	
    microdrive[m].transfered = 0; /* reset current number of bytes written */

    if( ( microdrive[m].head_pos % 543 ) == 0 ) {
      microdrive[m].max_bytes = 15; /* up to 15 bytes for header blocks */
    } else {
      microdrive[m].max_bytes = 528; /* up to 528 bytes for data blocks */
    }
  }	
}

void
if1_mdr_writep( int w, int drive )
{
  microdrive[drive].wp = w ? 1 : 0;
  microdrive[drive].modified = 1;

  update_menu( UMENU_MDRV1 + drive );
}

void
if1_mdr_new( int drive )
{
  if( microdrive[drive].inserted && if1_mdr_eject( NULL, drive ) ) {
    ui_error( UI_ERROR_ERROR,
	      "New cartridge in drive, please eject manually" );
    return;
  }

  /* Erase the entire cartridge */
  memset( microdrive[drive].cartridge, 0xff, CARTRIDGE_LEN );
  microdrive[drive].wp = 0; /* but don't write-protect */

  microdrive[drive].filename = NULL;
  microdrive[drive].inserted = 1;
  microdrive[drive].modified = 1;

  update_menu( UMENU_MDRV1 + drive );
}

void
if1_mdr_insert( char *filename, int drive )
{
  FILE *file;
  char wp;

  if( microdrive[drive].inserted && if1_mdr_eject( NULL, drive ) ) {
    ui_error( UI_ERROR_ERROR,
	      "New cartridge in drive, please eject manually" );
    return;
  }

  file = fopen( filename, "r" );
  if( !file ) {
    ui_error( UI_ERROR_ERROR, "Error opening '%s': %s", filename,
	      strerror( errno ) );
    return;
  }

  /* FIXME: should check for short read count; however, we'll replace
     this with libspectrum routines soon so probably not worth it */
  if( !fread( microdrive[drive].cartridge, CARTRIDGE_LEN, 1, file) ) {
    ui_error( UI_ERROR_ERROR, "Error reading data from '%s': %s", filename,
	      strerror( errno ) );
    return;
  }

  if( !fread( &wp, 1, 1, file) ) {
    ui_error( UI_ERROR_ERROR, "Error reading write protection flag '%s': %s",
	      filename, strerror( errno ) );
    return;
  }

  if( fclose( file ) ) {
    ui_error( UI_ERROR_ERROR, "Error closing '%s': %s", filename,
	      strerror( errno ) );
    return;
  }

  microdrive[drive].wp = wp ? 1 : 0;
  microdrive[drive].inserted = 1;

  microdrive[drive].filename = malloc( strlen( filename ) + 1 );
  if( microdrive[drive].filename ) {
    strcpy( microdrive[drive].filename, filename );
  } else {
    ui_error( UI_ERROR_ERROR, "Out of memory at %s:%d\n", __FILE__, __LINE__ );
  }
  
  update_menu( UMENU_MDRV1 + drive );
}

int
if1_mdr_sync( char *filename, int drive )
{
  FILE *file;
  char wp;
  
  if( microdrive[drive].modified ) {

    if( !microdrive[drive].filename ) {	/* New cartridge */
      if( !filename ) return 1;		/* We need a filename :-) */
    } else {
      filename = microdrive[drive].filename;
    }

    wp = microdrive[drive].wp ? 0xff : 0;
    file = fopen( filename, "w" );

    if( !file ) {
      ui_error( UI_ERROR_ERROR, "Error opening '%s': %s",
		filename, strerror( errno ) );
      return 1;
    }

  /* FIXME: should check for short write count; however, we'll replace
     this with libspectrum routines soon so probably not worth it */
    if( !fwrite( microdrive[drive].cartridge, CARTRIDGE_LEN, 1, file) ) {
      ui_error( UI_ERROR_ERROR, "Error writing data to '%s': %s",
		filename, strerror( errno ) );
      return 1;
    }

    if( !fwrite( &wp, 1, 1, file) ) {
      ui_error( UI_ERROR_ERROR, "Error writing write protection flag '%s': %s",
		filename, strerror( errno ) );
      return 1;
    }

    if( fclose( file ) ) {
      ui_error( UI_ERROR_ERROR, "Error closing '%s': %s",
		filename, strerror( errno ) );
      return 1;
    }

    if( !microdrive[drive].filename ) {	/* New cartridge */
      microdrive[drive].filename = malloc( strlen( filename ) + 1 );
      if( microdrive[drive].filename )
        strcpy( microdrive[drive].filename, filename );
    }
  }

  return 0;
}


int
if1_mdr_eject( char *filename, int drive )
{
  /* Report if we need a filename */
  if( if1_mdr_sync( filename, drive ) ) return 1;  

  microdrive[drive].inserted = 0;
  free( microdrive[drive].filename );
  microdrive[drive].filename = NULL;
  
  update_menu( UMENU_MDRV1 + drive );
  return 0;
}

void
if1_plug( char *filename, int what )
{
  int fd = -1;

  switch( what ) {
  case 1:
    if( if1_ula.fd_r >= 0 )
      close( if1_ula.fd_r );
    fd = if1_ula.fd_r = open( filename, O_RDONLY | O_NONBLOCK );
    break;
  case 2:
    if( if1_ula.fd_t >= 0 )
      close( if1_ula.fd_t );
    fd = if1_ula.fd_t = open( filename, O_WRONLY | O_NONBLOCK );
    if1_ula.dtr = fd == -1 ? 0 : 1;
    break;
  case 3:
    if( if1_ula.fd_net >= 0 )
      close( if1_ula.fd_net );
    fd = if1_ula.fd_net = open( filename, O_RDWR | O_NONBLOCK );
    break;
  }

  if( fd < 0 ) {
    ui_error( UI_ERROR_ERROR, "Error opening '%s': %s",
		filename, strerror( errno ) );
    return;
  }
  if1_ula.rs232_mode = settings_current.raw_rs232 ? RS232_RAW : RS232_INT;
  if1_ula.s_net_mode = settings_current.raw_s_net ? S_NET_RAW : S_NET_INT;
  update_menu( UMENU_RS232 );
}

void
if1_unplug( int what )
{
  switch( what ) {
  case 1:
    if( if1_ula.fd_r >= 0 )
      close( if1_ula.fd_r );
    if1_ula.fd_r = -1;
    break;
  case 2:
    if( if1_ula.fd_t >= 0 )
      close( if1_ula.fd_t );
    if1_ula.fd_t = -1;
    if1_ula.dtr = 0;
    break;
  case 3:
    if( if1_ula.fd_net >= 0 )
      close( if1_ula.fd_net );
    if1_ula.fd_net = -1;
    break;
  }
  update_menu( UMENU_RS232 );
}
