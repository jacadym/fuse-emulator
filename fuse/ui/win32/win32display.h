/* win32display.h:  Routines for dealing with the Win32 DirectDraw display
   Copyright (c) 2003 Marek Januszewski

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

#include "config.h"

#ifdef UI_WIN32			/* Use this iff we're using UI_WIN32 */

#include <windows.h>
#include <ddraw.h>

/* window handler */
HWND hWnd;

/* windows message */
MSG msg;

/* DirectDraw objects */
LPDIRECTDRAW lpdd;
DDSURFACEDESC ddsd;
LPDIRECTDRAWSURFACE lpdds, /* primary surface */
                    lpdds2; /* off screen surface */
LPDIRECTDRAWCLIPPER lpddc; /* clipper for the window */

/* context */
HDC dc;

void blit( void );

#endif			/* #ifdef UI_WIN32 */
