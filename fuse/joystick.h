/* joystick.h: Joystick emulation support
   Copyright (c) 2001-2004 Russell Marks, Philip Kendall
   Copyright (c) 2003 Darren Salt

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

#ifndef FUSE_JOYSTICK_H
#define FUSE_JOYSTICK_H

#include <libspectrum.h>

/* Number of joysticks known about & initialised */
extern int joysticks_supported;

/* Init/shutdown functions. Errors aren't important here */
void fuse_joystick_init( void );
void fuse_joystick_end( void );

typedef enum joystick_type_t {

  JOYSTICK_TYPE_NONE = 0,
  
  JOYSTICK_TYPE_CURSOR,
  JOYSTICK_TYPE_KEMPSTON,
  JOYSTICK_TYPE_TIMEX,

} joystick_type_t;

#define JOYSTICK_TYPE_COUNT 4

extern char *joystick_name[];

typedef enum joystick_button {

  JOYSTICK_BUTTON_RIGHT,
  JOYSTICK_BUTTON_LEFT,
  JOYSTICK_BUTTON_DOWN,
  JOYSTICK_BUTTON_UP,
  JOYSTICK_BUTTON_FIRE,

} joystick_button;

/* Called whenever the (Spectrum) joystick is moved or the fire button
   pressed */
int joystick_press( int which, joystick_button button, int press );

/* Interface-specific read functions */
libspectrum_byte joystick_kempston_read ( libspectrum_word port,
					  int *attached );
libspectrum_byte joystick_timex_read ( libspectrum_word port,
				       libspectrum_byte which );

#endif			/* #ifndef FUSE_JOYSTICK_H */
