#include <config.h>

#include <libspectrum.h>

#include "dck.h"
#include "event.h"
#include "fuse.h"
#include "menu.h"
#include "psg.h"
#include "rzx.h"
#include "simpleide.h"
#include "settings.h"
#include "tape.h"
#include "ui/ui.h"
#include "widget/widget.h"

#ifdef USE_WIDGET
#define WIDGET_END widget_finish()
#else				/* #ifdef USE_WIDGET */
#define WIDGET_END
#endif				/* #ifdef USE_WIDGET */

MENU_CALLBACK( menu_file_recording_stop )
{
  if( !( rzx_recording || rzx_playback ) ) return;

  WIDGET_END;

  if( rzx_recording ) rzx_stop_recording();
  if( rzx_playback  ) rzx_stop_playback( 1 );
}  

MENU_CALLBACK( menu_file_aylogging_stop )
{
  if ( !psg_recording ) return;

  WIDGET_END;

  psg_stop_recording();
  ui_menu_activate( UI_MENU_ITEM_AY_LOGGING, 0 );
}

MENU_CALLBACK_WITH_ACTION( menu_options_selectroms_select )
{
  switch( action ) {

  case  1: menu_select_roms( LIBSPECTRUM_MACHINE_16,      0, 1 ); return;
  case  2: menu_select_roms( LIBSPECTRUM_MACHINE_48,      1, 1 ); return;
  case  3: menu_select_roms( LIBSPECTRUM_MACHINE_128,     2, 2 ); return;
  case  4: menu_select_roms( LIBSPECTRUM_MACHINE_PLUS2,   4, 2 ); return;
  case  5: menu_select_roms( LIBSPECTRUM_MACHINE_PLUS2A,  6, 4 ); return;
  case  6: menu_select_roms( LIBSPECTRUM_MACHINE_PLUS3,  10, 4 ); return;
  case  7: menu_select_roms( LIBSPECTRUM_MACHINE_TC2048, 14, 1 ); return;
  case  8: menu_select_roms( LIBSPECTRUM_MACHINE_TC2068, 15, 2 ); return;
  case  9: menu_select_roms( LIBSPECTRUM_MACHINE_PENT,   17, 3 ); return;
  case 10: menu_select_roms( LIBSPECTRUM_MACHINE_SCORP,  20, 4 ); return;
  case 11: menu_select_roms( LIBSPECTRUM_MACHINE_PLUS3E, 24, 4 ); return;

  }

  ui_error( UI_ERROR_ERROR,
	    "menu_options_selectroms_select: unknown action %d", action );
  fuse_abort();
}

MENU_CALLBACK( menu_options_filter )
{
  scaler_type scaler;

  /* Stop emulation */
  fuse_emulation_pause();

  scaler = menu_get_scaler( scaler_is_supported );
  if( scaler != SCALER_NUM && scaler != current_scaler )
    scaler_select_scaler( scaler );

  /* Carry on with emulation again */
  fuse_emulation_unpause();
}

#ifdef HAVE_LIB_XML2
MENU_CALLBACK( menu_options_save )
{
  WIDGET_END;
  settings_write_config( &settings_current );
}
#endif				/* #ifdef HAVE_LIB_XML2 */

MENU_CALLBACK( menu_machine_nmi )
{
  WIDGET_END;
  event_add( 0, EVENT_TYPE_NMI );
}

MENU_CALLBACK( menu_media_tape_play )
{
  WIDGET_END;
  tape_toggle_play();
}

MENU_CALLBACK( menu_media_tape_rewind )
{
  WIDGET_END;
  tape_select_block( 0 );
}

MENU_CALLBACK( menu_media_tape_clear )
{
  WIDGET_END;
  tape_close();
}

MENU_CALLBACK_WITH_ACTION( menu_media_disk_eject )
{
  int which, write;

  WIDGET_END;

  action--;
  which = action & 0x01;
  write = action & 0x02;

#ifdef HAVE_765_H
  if( machine_current->capabilities &
      LIBSPECTRUM_MACHINE_CAPABILITY_PLUS3_DISK ) {
    specplus3_disk_eject( which, write );
    return;
  }
#endif			/* #ifdef HAVE_765_H */

  trdos_disk_eject( which, write );
}

MENU_CALLBACK( menu_media_cartridge_eject )
{
  WIDGET_END;
  dck_eject();
}

MENU_CALLBACK_WITH_ACTION( menu_media_ide_simple8bit_commit )
{
  fuse_emulation_pause();

  switch( action ) {
  case 1: simpleide_commit( LIBSPECTRUM_IDE_MASTER ); break;
  case 2: simpleide_commit( LIBSPECTRUM_IDE_SLAVE  ); break;
  }

  fuse_emulation_unpause();

  WIDGET_END;
}

MENU_CALLBACK_WITH_ACTION( menu_media_ide_simple8bit_eject )
{
  fuse_emulation_pause();

  switch( action ) {
  case 1: simpleide_eject( LIBSPECTRUM_IDE_MASTER ); break;
  case 2: simpleide_eject( LIBSPECTRUM_IDE_SLAVE  ); break;
  }

  fuse_emulation_unpause();

  WIDGET_END;
}
