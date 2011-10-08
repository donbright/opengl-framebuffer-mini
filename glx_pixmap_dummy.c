/*

set up an OpenGL contxet, by using glx pixmaps. This method requires
no onscreen 'dummy window' to open the context, just an offscreen
'dummy' pixmap.

based on http://www.opengl.org/sdk/docs/man/xhtml/glXIntro.xml
which was originally Copyright © 1991-2006 Silicon Graphics, Inc.
licensed under the SGI Free Software B License.
See http://oss.sgi.com/projects/FreeB/.

See also http://www.mesa3d.org/brianp/sig97/offscrn.htm

Note:

 Glx 1.2 and GLX 1.3 use different functions. For example:
  GLXCreatePixmap = GLX 1.3
  GLXCreateGLXPixmap = GLX 1.2

 If you mix them up, it can crash your X server completely.


*/

#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glx.h>

int singleBufferAttributess[] = {
	GLX_DRAWABLE_TYPE, GLX_PIXMAP_BIT,
	GLX_RENDER_TYPE,   GLX_RGBA_BIT,
	GLX_RED_SIZE, 1,
	GLX_GREEN_SIZE, 1,
	GLX_BLUE_SIZE, 1,
	None
};

int glx_pixmap_dummy()
{
	Display	*dpy;
	XVisualInfo *vInfo;
	GLXFBConfig *fbConfigs;
	GLXContext context;
	GLXPixmap glx_pixmap;
	Pixmap x11_pixmap;

	int numReturned;
	int result;

	dpy = XOpenDisplay( NULL );
	if ( dpy == NULL ) {
		printf( "Unable to open a connection to the X server\n" );
	return 0;
	}

	fbConfigs = glXChooseFBConfig( dpy, DefaultScreen(dpy),
					 singleBufferAttributess, 
					&numReturned );

	vInfo = glXGetVisualFromFBConfig( dpy, fbConfigs[0] );

	x11_pixmap = XCreatePixmap( dpy, RootWindow(dpy,vInfo->screen) , 10, 10, 8 );

	glx_pixmap = glXCreatePixmap( dpy, fbConfigs[0], x11_pixmap, NULL );

        context = glXCreateNewContext( dpy, fbConfigs[0], GLX_RGBA_TYPE, NULL, True );

	glXMakeContextCurrent( dpy, glx_pixmap, glx_pixmap, context );

	return 1;
}

