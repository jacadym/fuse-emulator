/* line.h: routines for dealing with a program line
   Copyright (C) 2002-2004 Philip Kendall

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

#ifndef BASIC_LINE_H
#define BASIC_LINE_H

#include <glib.h>

struct line;

#include "basic.h"
#include "statement.h"

struct line {

  int line_number;
  GSList* statements;

};

error_t line_append_statement( struct line *line,
			       struct statement *statement );
void line_free( gpointer data, gpointer user_data );

#endif				/* #ifndef BASIC_LINE_H */