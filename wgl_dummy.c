/*
based on http://www.opengl.org/sdk/docs/man/xhtml/glXIntro.xml
which was originally Copyright © 1991-2006 Silicon Graphics, Inc.
licensed under the SGI Free Software B License.
See http://oss.sgi.com/projects/FreeB/.
*/

#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glx.h>

int singleBufferAttributess[] = {
	GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
	GLX_RENDER_TYPE,   GLX_RGBA_BIT,
	GLX_RED_SIZE,	  1,   
	GLX_GREEN_SIZE,	1,   
	GLX_BLUE_SIZE,	 1,   
	None
};


static Bool WaitForNotify( Display *dpy, XEvent *event, XPointer arg ) {
	return (event->type == MapNotify) && (event->xmap.window == (Window) arg);
}
int glx_dummy()
{
	Display			  *dpy;
	Window				xWin;
	XEvent				event;
	XVisualInfo		  *vInfo;
	XSetWindowAttributes  swa;
	GLXFBConfig		  *fbConfigs;
	GLXContext			context;
	GLXWindow			 glxWin;
	int				   swaMask;
	int				   numReturned;

	dpy = XOpenDisplay( NULL );
	if ( dpy == NULL ) {
		printf( "Unable to open a connection to the X server\n" );
	return 0;
	}

	fbConfigs = glXChooseFBConfig( dpy, DefaultScreen(dpy),
					 singleBufferAttributess, &numReturned );

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

	glxWin = glXCreateWindow( dpy, fbConfigs[0], xWin, NULL );

	XMapWindow( dpy, xWin );
	XIfEvent( dpy, &event, WaitForNotify, (XPointer) xWin );

	glXMakeContextCurrent( dpy, glxWin, glxWin, context );
	return 1;
}

