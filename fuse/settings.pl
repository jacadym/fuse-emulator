#!/usr/bin/perl -w

# settings.pl: generate settings.c from settings.dat
# Copyright (c) 2002 Philip Kendall

# $Id$

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 49 Temple Place, Suite 330, Boston, MA 02111-1307 USA

# Author contact information:

# E-mail: pak21-fuse@srcf.ucam.org
# Postal address: 15 Crescent Road, Wokingham, Berks, RG40 2DB, England

use strict;

use lib 'perl';

use Fuse;

sub hashline ($) { "#line $_[0] \"", __FILE__, "\"\n" }

my %options;

while(<>) {

    next if /^\s*$/;
    next if /^\s*#/;

    chomp;

    my( $name, $type, $default, $short, $commandline, $configfile ) =
	split /\s*,\s*/;

    if( not defined $commandline ) {
	$commandline = $name;
	$commandline =~ s/_/-/g;
    }

    if( not defined $configfile ) {
	$configfile = $commandline;
	$configfile =~ s/-//g;
    }

    $options{$name} = { type => $type, default => $default, short => $short,
			commandline => $commandline,
			configfile => $configfile };
}

print Fuse::GPL( 'settings.c: Handling configuration settings',
		 'Copyright (c) 2002 Philip Kendall' );

print hashline( __LINE__ ), << 'CODE';

/* This file is autogenerated from settings.dat by settings.pl.
   Do not edit unless you know what will happen! */

#include <config.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef HAVE_GETOPT_LONG		/* Did our libc include getopt_long? */
#include <getopt.h>
#else				/* #ifdef HAVE_GETOPT_LONG */
#include "getopt/getopt.h"	/* If not, use ours */
#endif				/* #ifdef HAVE_GETOPT_LONG */

#ifdef HAVE_LIB_XML2
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#endif				/* #ifdef HAVE_LIB_XML2 */

#include "fuse.h"
#include "machine.h"
#include "settings.h"
#include "spectrum.h"
#include "ui/ui.h"

/* The current settings of options, etc */
settings_info settings_current;

#ifdef HAVE_LIB_XML2
static int read_config_file( settings_info *settings );
static int parse_xml( xmlDocPtr doc, settings_info *settings );
#endif				/* #ifdef HAVE_LIB_XML2 */

static int settings_command_line( settings_info *settings, int *first_arg,
				  int argc, char **argv );

/* Called on emulator startup */
int
settings_init( int *first_arg, int argc, char **argv )
{
  int error;

  error = settings_defaults( &settings_current );
  if( error ) return error;

#ifdef HAVE_LIB_XML2
  error = read_config_file( &settings_current );
  if( error ) return error;
#endif				/* #ifdef HAVE_LIB_XML2 */

  error = settings_command_line( &settings_current, first_arg, argc, argv );
  if( error ) return error;

  return 0;
}

/* Fill the settings structure with sensible defaults */
int settings_defaults( settings_info *settings )
{
CODE

    foreach my $name ( sort keys %options ) {
	print "  settings->$name = $options{$name}->{default};\n";
    }

    print hashline( __LINE__ ), << 'CODE';
  
#ifdef HAVE_LIBZ
  settings->rzx_compression = 1;
#endif			/* #ifdef HAVE_LIBZ */

  if( !settings->start_machine ) return 1;

  return 0;
}

#ifdef HAVE_LIB_XML2

/* Read options from the config file (if libxml2 is available) */

static int
read_config_file( settings_info *settings )
{
  const char *home; char path[256];
  struct stat stat_info;

  xmlDocPtr doc;

  home = getenv( "HOME" );
  if( !home ) {
    ui_error( UI_ERROR_ERROR, "couldn't get your home directory" );
    return 1;
  }

  snprintf( path, 256, "%s/.fuserc", home );

  /* See if the file exists; if doesn't, it's not a problem */
  if( stat( path, &stat_info ) ) {
    if( errno == ENOENT ) {
      return 0;
    } else {
      ui_error( UI_ERROR_ERROR, "couldn't stat '%s': %s", path,
		strerror( errno ) );
      return 1;
    }
  }

  doc = xmlParseFile( path );
  if( !doc ) {
    ui_error( UI_ERROR_ERROR, "error reading config file" );
    return 1;
  }

  if( parse_xml( doc, settings ) ) { xmlFreeDoc( doc ); return 1; }

  xmlFreeDoc( doc );

  return 0;
}

static int
parse_xml( xmlDocPtr doc, settings_info *settings )
{
  xmlNodePtr node;

  node = xmlDocGetRootElement( doc );
  if( xmlStrcmp( node->name, (const xmlChar*)"settings" ) ) {
    ui_error( UI_ERROR_ERROR, "config file's root node is not 'settings'" );
    return 1;
  }

  node = node->xmlChildrenNode;
  while( node ) {

CODE

foreach my $name ( sort keys %options ) {

    my $type = $options{$name}->{type};

    if( $type eq 'boolean' or $type eq 'numeric' ) {

	print << "CODE";
    if( !strcmp( node->name, (const xmlChar*)"$options{$name}->{configfile}" ) ) {
      settings->$name = atoi( xmlNodeListGetString( doc, node->xmlChildrenNode, 1 ) );
    } else
CODE

    } elsif( $type eq 'string' ) {

	    print << "CODE";
    if( !strcmp( node->name, (const xmlChar*)"$options{$name}->{configfile}" ) ) {
      settings->$name = strdup( xmlNodeListGetString( doc, node->xmlChildrenNode, 1 ) );
    } else
CODE
    } else {
	die "Unknown setting type `$type'";
    }
}

print hashline( __LINE__ ), << 'CODE';
    if( !strcmp( node->name, (const xmlChar*)"text" ) ) {
      /* Do nothing */
    } else {
      ui_error( UI_ERROR_ERROR, "Unknown setting '%s' in config file",
		node->name );
      return 1;
    }

    node = node->next;
  }

  return 0;
}

int
settings_write_config( settings_info *settings )
{
  const char *home; char path[256], buffer[80]; 

  xmlDocPtr doc; xmlNodePtr root;

  home = getenv( "HOME" );
  if( !home ) {
    ui_error( UI_ERROR_ERROR, "couldn't get your home directory" );
    return 1;
  }

  snprintf( path, 256, "%s/.fuserc", home );

  /* Create the XML document */
  doc = xmlNewDoc( "1.0" );

  root = xmlNewNode( NULL, "settings" );
  xmlDocSetRootElement( doc, root );
CODE

foreach my $name ( sort keys %options ) {

    my $type = $options{$name}->{type};

    if( $type eq 'boolean' ) {

	print "  xmlNewTextChild( root, NULL, \"$options{$name}->{configfile}\", settings->$name ? \"1\" : \"0\" );\n";

    } elsif( $type eq 'string' ) {
	print << "CODE";
  if( settings->$name )
    xmlNewTextChild( root, NULL, "$options{$name}->{configfile}", settings->$name );
CODE
    } elsif( $type eq 'numeric' ) {
	print << "CODE";
  if( settings->$name ) {
    snprintf( buffer, 80, "%d", settings->$name );
    xmlNewTextChild( root, NULL, "$options{$name}->{configfile}", buffer );
  }
CODE
    } else {
	die "Unknown setting type `$type'";
    }
}

  print hashline( __LINE__ ), << 'CODE';

  xmlSaveFormatFile( path, doc, 1 );

  return 0;
}

#endif				/* #ifdef HAVE_LIB_XML2 */

/* Read options from the command line */
static int
settings_command_line( settings_info *settings, int *first_arg,
                       int argc, char **argv )
{

  struct option long_options[] = {

CODE

my $fake_short_option = 256;

foreach my $name ( sort keys %options ) {

    my $type = $options{$name}->{type};
    my $commandline = $options{$name}->{commandline};
    my $short = $options{$name}->{short};

    unless( $type eq 'boolean' or $short ) { $short = $fake_short_option++ }

    if( $type eq 'boolean' ) {

	print << "CODE";
    {    "$commandline", 0, &(settings->$name), 1 },
    { "no-$commandline", 0, &(settings->$name), 0 },
CODE
    } elsif( $type eq 'string' or $type eq 'numeric' ) {

	print "    { \"$commandline\", 1, NULL, $short },\n";
    } else {
	die "Unknown setting type `$type'";
    }
}

print hashline( __LINE__ ), << 'CODE';

    { "help", 0, NULL, 'h' },
    { "version", 0, NULL, 'V' },

    { 0, 0, 0, 0 }		/* End marker: DO NOT REMOVE */
  };

  while( 1 ) {

    int c;

    c = getopt_long( argc, argv, "d:hm:o:p:r:s:t:v:V", long_options, NULL );

    if( c == -1 ) break;	/* End of option list */

    switch( c ) {

    case 0: break;	/* Used for long option returns */

CODE

$fake_short_option = 256;

foreach my $name ( sort keys %options ) {

    my $type = $options{$name}->{type};
    my $short = $options{$name}->{short};

    unless( $type eq 'boolean' or $short ) { $short = $fake_short_option++ }

    if( $type eq 'boolean' ) {
	# Do nothing
    } elsif( $type eq 'string' ) {
	print "    case $short: settings->$name = optarg; break;\n";
    } elsif( $type eq 'numeric' ) {
	print "    case $short: settings->$name = atoi( optarg ); break;\n";
    } else {
	die "Unknown setting type `$type'";
    }
}

print hashline( __LINE__ ), << 'CODE';

    case 'h': settings->show_help = 1; break;
    case 'V': settings->show_version = 1; break;

    case ':':
    case '?':
      break;

    default:
      fprintf( stderr, "%s: getopt_long returned `%c'\n",
	       fuse_progname, (char)c );
      break;

    }
  }

  /* Store the location of the first non-option argument */
  *first_arg = optind;

  return 0;
}

/* Copy one settings object to another */
int settings_copy( settings_info *dest, settings_info *src )
{
  if( settings_defaults( dest ) ) return 1;
  free( dest->start_machine ); dest->start_machine = NULL;

CODE

foreach my $name ( sort keys %options ) {

    my $type = $options{$name}->{type};

    if( $type eq 'boolean' or $type eq 'numeric' ) {
	print "  dest->$name = src->$name;\n";
    } elsif( $type eq 'string' ) {
	print << "CODE";
  if( src->$name ) {
    dest->$name = strdup( src->$name );
    if( !dest->$name ) { settings_free( dest ); return 1; }
  }
CODE
    }
}

print hashline( __LINE__ ), << 'CODE';

  return 0;
}

int
settings_set_string( char **string_setting, char *value )
{
  if( *string_setting) free( *string_setting );
  *string_setting = strdup( value );
  if( !( *string_setting ) ) {
    ui_error( UI_ERROR_ERROR, "out of memory at %s:%d", __FILE__, __LINE__ );
    return 1;
  }

  return 0;
}

int
settings_free( settings_info *settings )
{
CODE

foreach my $name ( sort keys %options ) {
    if( $options{$name}->{type} eq 'string' ) {
	print "  if( settings->$name ) free( settings->$name );\n";
    }
}

print hashline( __LINE__ ), << 'CODE';

  return 0;
}
CODE
