/* rzx.h: .rzx files
   Copyright (c) 2002 Philip Kendall

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

#ifndef FUSE_RZX_H
#define FUSE_RZX_H

/* The count of instruction fetches needed for .rzx files */
extern size_t rzx_instructions;

/* Are we currently recording a .rzx file? */
extern int rzx_recording;

int rzx_init( void );

int rzx_start_recording( void );
int rzx_stop_recording( void );

int rzx_frame( void );
int rzx_dump( void );

#endif			/* #ifndef FUSE_RZX_H */
