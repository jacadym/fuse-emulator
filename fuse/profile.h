/* profile.h: Z80 profiler
   Copyright (c) 2005 Philip Kendall
		 
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

   E-mail: Philip Kendall <philip-fuse@shadowmagic.org.uk>

*/

#ifndef FUSE_PROFILE_H
#define FUSE_PROFILE_H

extern int profile_active;

void profile_start( void );
void profile_map( libspectrum_word pc );
void profile_frame( libspectrum_dword frame_length );
void profile_finish( const char *filename );

#endif			/* #ifndef FUSE_PROFILE_H */
