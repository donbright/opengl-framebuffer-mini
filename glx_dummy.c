#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glx.h>

/* part of this file is writen by Don B 2011, put into the public domain */

/* the below copyright notice by Brian Pual is for the make_window function and glxgears dummy function only */

/*
 * Copyright (C) 1999-2001  Brian Paul   All Rights Reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * BRIAN PAUL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * This is a port of the infamous "gears" demo to straight GLX (i.e. no GLUT)
 * Port by Brian Paul  23 March 2001
 */

static void
make_window( Display *dpy, const char *name,
             int x, int y, int width, int height,
             Window *winRet, GLXContext *ctxRet)
{
   int attribs[] = { GLX_RGBA,
                     GLX_RED_SIZE, 1,
                     GLX_GREEN_SIZE, 1,
                     GLX_BLUE_SIZE, 1,
                     GLX_DOUBLEBUFFER,
                     GLX_DEPTH_SIZE, 1,
                     None };

   int scrnum;
   XSetWindowAttributes attr;
   unsigned long mask;
   Window root;
   Window win;
   GLXContext ctx;
   XVisualInfo *visinfo;
   Pixmap x11_pixmap;
   GLXPixmap glx_pixmap;

   scrnum = DefaultScreen( dpy );
   root = RootWindow( dpy, scrnum );

   visinfo = glXChooseVisual( dpy, scrnum, attribs );
   if (!visinfo) {
      printf("Error: couldn't get an RGB, Double-buffered visual\n");
      exit(1);
   }

   /* window attributes */
   attr.background_pixel = 0;
   attr.border_pixel = 0;
   attr.colormap = XCreateColormap( dpy, root, visinfo->visual, AllocNone);
   attr.event_mask = StructureNotifyMask | ExposureMask | KeyPressMask;
   /* XXX this is a bad way to get a borderless window! */
   mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

   win = XCreateWindow( dpy, root, x, y, width, height,
		        0, visinfo->depth, InputOutput,
		        visinfo->visual, mask, &attr );

   x11_pixmap = XCreatePixmap( dpy, root, width, height, visinfo->depth );
   glx_pixmap = glXCreateGLXPixmap( dpy, visinfo, x11_pixmap );

   ctx = glXCreateContext( dpy, visinfo, NULL, True );
   if (!ctx) {
      printf("Error: glXCreateContext failed\n");
      exit(1);
   }

   XFree(visinfo);

   // *winRet = win;
   *winRet = glx_pixmap;
   *ctxRet = ctx;
}

int glxgears_dummy()
{
   unsigned int winWidth = 300, winHeight = 300;
   int x = 0, y = 0;
   Display *dpy;
   Window win;
   GLXContext ctx;
   char *dpyName = NULL;
   GLboolean printInfo = GL_FALSE;
   int i;

   dpy = XOpenDisplay(dpyName);
   if (!dpy) {
      printf("Error: couldn't open display %s\n",
	     dpyName ? dpyName : getenv("DISPLAY"));
      return -1;
   }

   make_window(dpy, "glxgears", x, y, winWidth, winHeight, &win, &ctx);
   //XMapWindow(dpy, win);
   glXMakeCurrent(dpy, win, ctx);

//  glXMakeCurrent(dpy, None, NULL);
//   glXDestroyContext(dpy, ctx);
//   XDestroyWindow(dpy, win);
//   XCloseDisplay(dpy);

   return 0;
}





/*
based on http://www.opengl.org/sdk/docs/man/xhtml/glXIntro.xml
which was originally Copyright © 1991-2006 Silicon Graphics, Inc.
licensed under the SGI Free Software B License.
See http://oss.sgi.com/projects/FreeB/.
*/


int singleBufferAttributess[] = {
	GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
	GLX_RENDER_TYPE,   GLX_RGBA_BIT,
	GLX_RED_SIZE, 1,
	GLX_GREEN_SIZE, 1,
	GLX_BLUE_SIZE, 1,
	None
};

Bool WaitForNotify(Display *d, XEvent *e, char *arg)
{
return (e->type == MapNotify) && (e->xmap.window == (Window)arg);
}

int glx_dummy()
{
//	glxgears_dummy();
//	return 0;
	Display	*dpy;
	Window xWin;
	XEvent event;
	XVisualInfo *vInfo;
	XSetWindowAttributes swa;
	GLXFBConfig *fbConfigs;
	GLXContext context;
	GLXWindow glxWin;
	int swaMask;
	int numReturned;

	dpy = XOpenDisplay( NULL );
	if ( dpy == NULL ) {
		printf( "Unable to open a connection to the X server\n" );
	return 0;
	}

	fbConfigs = glXChooseFBConfig( dpy, DefaultScreen(dpy),
					 singleBufferAttributess, 
					&numReturned );

	vInfo = glXGetVisualFromFBConfig( dpy, fbConfigs[0] );

	swa.border_pixel = 0;
	swa.event_mask = StructureNotifyMask;
	swa.colormap = XCreateColormap( dpy, RootWindow(dpy, vInfo->screen),
						vInfo->visual, AllocNone );
	swaMask = CWBorderPixel | CWColormap | CWEventMask;
	xWin = XCreateWindow( dpy, RootWindow(dpy, vInfo->screen), 0, 0, 10, 10,
						  0, vInfo->depth, InputOutput, vInfo->visual,
						  swaMask, &swa );

	context = glXCreateNewContext( dpy, fbConfigs[0], GLX_RGBA_TYPE,
				 NULL, True );

//	glxWin = glXCreateWindow( dpy, fbConfigs[0], xWin, NULL );

	//XMapWindow( dpy, xWin );
	//XIfEvent( dpy, &event, WaitForNotify, (XPointer) xWin );
        XSync( dpy, 1);

	glXMakeContextCurrent( dpy, xWin, xWin, context );
	return 1;
}





