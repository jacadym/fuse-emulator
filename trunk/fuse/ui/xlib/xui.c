/* xui.c: Routines for dealing with the Xlib user interface
   Copyright (c) 2000-2003 Philip Kendall

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

#ifdef UI_X			/* Use this iff we're using Xlib */

#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "display.h"
#include "fuse.h"
#include "keyboard.h"
#include "settings.h"
#include "ui/ui.h"
#include "ui/uidisplay.h"
#include "xdisplay.h"
#include "xkeyboard.h"
#include "xui.h"

Display *display;		/* Which display are we connected to */
int xui_screenNum;		/* Which screen are we using on our
				   X server? */
Window xui_mainWindow;		/* Window ID for the main Fuse window */

/* FIXME: not a prototype. What should it be? */
Bool xui_trueFunction();

int
ui_init( int *argc, char ***argv )
{
  char *displayName=NULL;	/* Use default display */
  XWMHints *wmHints;
  XSizeHints *sizeHints;
  XClassHint *classHint;
  char *windowNameList="Fuse",*iconNameList="Fuse";
  XTextProperty windowName, iconName;
  unsigned long windowFlags;
  XSetWindowAttributes windowAttributes;

  /* Allocate memory for various things */

  if(!(wmHints = XAllocWMHints())) {
    fprintf(stderr,"%s: failure allocating memory\n", fuse_progname);
    return 1;
  }

  if(!(sizeHints = XAllocSizeHints())) {
    fprintf(stderr,"%s: failure allocating memory\n", fuse_progname);
    return 1;
  }

  if(!(classHint = XAllocClassHint())) {
    fprintf(stderr,"%s: failure allocating memory\n", fuse_progname);
    return 1;
  }

  if(XStringListToTextProperty(&windowNameList,1,&windowName) == 0 ) {
    fprintf(stderr,"%s: structure allocation for windowName failed\n",
	    fuse_progname);
    return 1;
  }

  if(XStringListToTextProperty(&iconNameList,1,&iconName) == 0 ) {
    fprintf(stderr,"%s: structure allocation for iconName failed\n",
	    fuse_progname);
    return 1;
  }

  /* Open a connection to the X server */

  if ( ( display=XOpenDisplay(displayName)) == NULL ) {
    fprintf(stderr,"%s: cannot connect to X server %s\n", fuse_progname,
	    XDisplayName(displayName));
    return 1;
  }

  /* Set up our error handler */
  xerror_expecting = xerror_error = 0;
  XSetErrorHandler( xerror_handler );

  xui_screenNum=DefaultScreen(display);

  /* Create the main window */

  xui_mainWindow = XCreateSimpleWindow(
    display, RootWindow( display, xui_screenNum ), 0, 0,
    DISPLAY_ASPECT_WIDTH, DISPLAY_SCREEN_HEIGHT, 0,
    BlackPixel( display, xui_screenNum ), WhitePixel( display, xui_screenNum )
  );

  /* Set standard window properties */

  sizeHints->flags = PBaseSize | PResizeInc | PMaxSize;
  sizeHints->base_width=0;
  sizeHints->base_height=0;

  sizeHints->width_inc    =     DISPLAY_ASPECT_WIDTH;
  sizeHints->height_inc   =     DISPLAY_SCREEN_HEIGHT;
  sizeHints->max_width    = 2 * DISPLAY_ASPECT_WIDTH;
  sizeHints->max_height   = 2 * DISPLAY_SCREEN_HEIGHT;

  if( settings_current.aspect_hint ) {
    sizeHints->flags |= PAspect;
    sizeHints->min_aspect.x = DISPLAY_ASPECT_WIDTH;
    sizeHints->min_aspect.y = DISPLAY_SCREEN_HEIGHT;
    sizeHints->max_aspect.x = DISPLAY_ASPECT_WIDTH;
    sizeHints->max_aspect.y = DISPLAY_SCREEN_HEIGHT;
  }

  wmHints->flags=StateHint | InputHint;
  wmHints->initial_state=NormalState;
  wmHints->input=True;

  classHint->res_name=fuse_progname;
  classHint->res_class="Fuse";

  XSetWMProperties(display, xui_mainWindow, &windowName, &iconName,
		   *argv, *argc, sizeHints, wmHints, classHint);

  /* Ask the server to use its backing store for this window */

  windowFlags=CWBackingStore;
  windowAttributes.backing_store=WhenMapped;

  XChangeWindowAttributes(display, xui_mainWindow, windowFlags,
			  &windowAttributes);

  /* Select which types of event we want to receive */

  XSelectInput(display, xui_mainWindow, ExposureMask | KeyPressMask |
	       KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
	       StructureNotifyMask | FocusChangeMask );

  if( xdisplay_init() ) return 1;

  /* And finally display the window */
  XMapWindow(display,xui_mainWindow);

  return 0;
}

int ui_event(void)
{
  XEvent event;

  while(XCheckIfEvent(display,&event,xui_trueFunction,NULL)) {
    switch(event.type) {
    case ConfigureNotify:
      xdisplay_configure_notify(event.xconfigure.width,
				event.xconfigure.height);
      break;
    case Expose:
      xdisplay_area( event.xexpose.x, event.xexpose.y,
		     event.xexpose.width, event.xexpose.height );
      break;
    case FocusOut:
      keyboard_release_all();
      break;
    case KeyPress:
      xkeyboard_keypress(&(event.xkey));
      break;
    case KeyRelease:
      xkeyboard_keyrelease(&(event.xkey));
      break;
    }
  }
  return 0;
}
    
/* FIXME: still doesn't have a prototype */
Bool xui_trueFunction()
{
  return True;
}

int ui_end(void)
{
  int error;

  /* Don't display the window whilst doing all this */
  XUnmapWindow(display,xui_mainWindow);

  /* Tidy up the low level stuff */
  error = xdisplay_end(); if( error ) return error;

  /* Now free up the window itself */
  XDestroyWindow(display,xui_mainWindow);

  /* And disconnect from the X server */
  XCloseDisplay(display);

  return 0;
}

#endif				/* #ifdef UI_X */
