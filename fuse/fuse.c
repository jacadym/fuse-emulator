/* fuse.c: The Free Unix Spectrum Emulator
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
#include <unistd.h>

#include "display.h"
#include "event.h"
#include "fuse.h"
#include "keyboard.h"
#include "machine.h"
#include "printer.h"
#include "rzx.h"
#include "settings.h"
#include "snapshot.h"
#include "sound.h"
#include "spectrum.h"
#include "tape.h"
#include "timer.h"
#include "ui/ui.h"
#include "widget/widget.h"
#include "z80/z80.h"

/* What name were we called under? */
char* fuse_progname;

/* A flag to say when we want to exit the emulator */
int fuse_exiting;

/* Is Spectrum emulation currently paused, and if so, how many times? */
int fuse_emulation_paused;

/* Are we going to try and use the sound card; this differs from
   sound.c:sound_enabled in that this gives a desire, whereas sound_enabled
   is an actual state; when the Spectrum emulation is not running, this
   stores whether we try to reenable the sound card afterwards */
int fuse_sound_in_use;

static int fuse_init(int argc, char **argv);

static void fuse_show_copyright(void);
static void fuse_show_version( void );
static void fuse_show_help( void );

static int fuse_end(void);

int main(int argc,char **argv)
{
  if(fuse_init(argc,argv)) {
    fprintf(stderr,"%s: error initalising -- giving up!\n", fuse_progname);
    return 1;
  }

  if( settings_current.show_help ||
      settings_current.show_version ) return 0;

  while( !fuse_exiting ) {
    z80_do_opcodes();
    event_do_events();
  }

  fuse_end();
  
  return 0;

}

static int fuse_init(int argc, char **argv)
{
  int error;

  fuse_progname=argv[0];
  
  if( settings_init( argc, argv ) ) return 1;

  if( settings_current.show_version ) {
    fuse_show_version();
    return 0;
  } else if( settings_current.show_help ) {
    fuse_show_help();
    return 0;
  }

  fuse_show_copyright();

  /* FIXME: order of these initialisation calls. Work out what depends on
     what */
  fuse_keyboard_init();

  if( tape_init() ) return 1;

  if( widget_init() ) return 1;

  if(display_init(&argc,&argv)) return 1;

  /* Drop root privs if we have them */
  if( !geteuid() ) { setuid( getuid() ); }

  if(event_init()) return 1;
  
  if( printer_init() ) return 1;
  if( rzx_init() ) return 1;

  settings_current.sound = fuse_sound_in_use = 0;
  sound_init( settings_current.sound_device );
  if(sound_enabled) {
    settings_current.sound = fuse_sound_in_use = 1;
  } else {
    if(timer_init()) return 1;
  }

  z80_init();

  error = machine_init_machines();
  if( error ) return error;
  error = machine_select_id( settings_current.start_machine );
  if( error ) return error;

  if( settings_current.snapshot ) snapshot_read( settings_current.snapshot );
  if( settings_current.tape_file ) tape_open( settings_current.tape_file );

  if( settings_current.playback_file ) {
    rzx_start_playback( settings_current.playback_file, NULL );
  } else if( settings_current.record_file ) {
    rzx_start_recording( settings_current.record_file, 1 );
  }

  fuse_emulation_paused = 0;

  return 0;

}

static void fuse_show_copyright(void)
{
  printf( "\n" );
  fuse_show_version();
  printf(
   "Copyright (c) 1999-2002 Philip Kendall <pak21-fuse@srcf.ucam.org> and others;\n"
   "See the file `AUTHORS' for more details.\n"
   "\n"
   "This program is distributed in the hope that it will be useful,\n"
   "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
   "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
   "GNU General Public License for more details.\n\n");
}

static void fuse_show_version( void )
{
  printf( "The Free Unix Spectrum Emulator (Fuse) version " VERSION ".\n" );
}

static void fuse_show_help( void )
{
  printf( "\n" );
  fuse_show_version();
  printf(
   "\nAvailable command-line options:\n\n"
   "Boolean options (use `--no-<option>' to turn off):\n\n"
   "--issue2               Emulate an Issue 2 Spectrum.\n"
   "--kempston             Emulate the Kempston joystick on QAOP<space>.\n"
   "--separation           Use ACB stereo for the AY-3-8912 sound chip.\n"
   "--traps                Turn tape traps on.\n\n"
   "Other options:\n\n"
   "--help                 This information.\n"
   "--snapshot <filename>  Load snapshot <filename>.\n"
   "--tape <filename>      Open tape file <filename>.\n"
   "--version              Print version number and exit.\n\n" );
}

/* Stop all activities associated with actual Spectrum emulation */
int fuse_emulation_pause(void)
{
  /* If we were already paused, just return. In any case, increment
     the pause count */
  if( fuse_emulation_paused++ ) return 0;

  /* If we had sound enabled (and hence doing the speed regulation),
     turn it off */
  if( sound_enabled ) sound_end();

  return 0;
}

/* Restart emulation activities */
int fuse_emulation_unpause(void)
{
  /* If this doesn't start us running again, just return. In any case,
     decrement the pause count */
  if( --fuse_emulation_paused ) return 0;

  /* If we now want sound, enable it */
  if( settings_current.sound ) {

    /* If sound wasn't in use before, remove the old SIGALRM timer */
    if( !fuse_sound_in_use ) timer_end();

    sound_init( settings_current.sound_device );

    /* If the sound code couldn't re-initialise, fall back to the
       signal based routines */
    if( !sound_enabled ) {
      /* Increment pause_count, report, decrement pause_count
       * (i.e. avoid the effects of fuse_emulation_{,un}pause).
       * Otherwise, we may be recursively reporting this error. */
      fuse_emulation_paused++;
      ui_error( UI_ERROR_ERROR, "Couldn't reinitialise sound" );
      fuse_emulation_paused--;
      settings_current.sound = fuse_sound_in_use = 0;
      /* FIXME: How to deal with error return here? */
      timer_init();

    }
    fuse_sound_in_use = sound_enabled;
  }
  /* If we don't want sound any more, put previously did, start the SIGALRM
     timer */
  else if( fuse_sound_in_use ) {
    timer_init();
    fuse_sound_in_use = 0;
  }

  return 0;
}

/* Tidy-up function called at end of emulation */
static int fuse_end(void)
{
  int error;

  /* Must happen before memory is deallocated as we read the character
     set from memory for the text output */
  printer_end();

  error = machine_end();
  if( error ) return error;

  if(!sound_enabled) timer_end();
  sound_end();
  rzx_end();
  event_end();
  ui_end();

  widget_end();

  return 0;
}

/* Emergency shutdown */
int fuse_abort( void )
{
  fuse_end();
  abort();
}
