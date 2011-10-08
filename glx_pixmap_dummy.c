/*

set up an OpenGL contxet, by using glx pixmaps. This method requires
no onscreen 'dummy window' to open the context, just an offscreen
'dummy' pixmap.

based on http://www.opengl.org/sdk/docs/man/xhtml/glXIntro.xml
which was originally Copyright © 1991-2006 Silicon Graphics, Inc.
licensed under the SGI Free Software B License.
See http://oss.sgi.com/projects/FreeB/.

See also http://www.mesa3d.org/brianp/sig97/offscrn.htm

See main.c for links to GLX 1.2 vs 1.3 issues

*/

#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glx.h>

void dump_vinfo(XVisualInfo *vi)
{
	//Visual *visual;
	//VisualID visualid;
	printf("visual ID macro: %i ",(int)XVisualIDFromVisual(vi->visual));
	printf("visualid: %i\n",(int)vi->visualid);
	printf("screen: %i ",vi->screen);
	printf("depth: %i ",vi->depth);
	printf("class: %i ",vi->class);
	printf("bits per rgb: %i\n",vi->bits_per_rgb);
	printf("red mask: %lu ",vi->red_mask);
	printf("green mask: %lu ",vi->green_mask);
	printf("blue mask: %lu ",vi->blue_mask);
	printf("colormap size: %i\n",vi->colormap_size);
}

int glx_1_2_pixmap_dummy(Display *dpy)
{
    /* untested. based on http://glprogramming.com/blue/ch07.html */
    XVisualInfo *vi;
    GLXContext cx;
    GLXPixmap glx_pixmap;
    Pixmap x11_pixmap;
    int dummyAttributes_1_2[] = {
        GLX_RGBA,
        None
    };

    dpy = XOpenDisplay(0);

    vi = glXChooseVisual(dpy, DefaultScreen(dpy), dummyAttributes_1_2 );

    dump_vinfo(vi);

    x11_pixmap = XCreatePixmap( dpy, DefaultRootWindow(dpy) , 10, 10, 32 );

    glx_pixmap = glXCreateGLXPixmap( dpy, vi, x11_pixmap );

    cx = glXCreateContext(dpy, vi, 0, GL_FALSE);
    glXMakeCurrent(dpy, glx_pixmap, cx);
}

int glx_1_3_pixmap_dummy(Display *dpy, Bool hybrid)
{
	XVisualInfo *vInfo;
	GLXFBConfig *fbConfigs;
	GLXContext context;
	GLXPixmap glx_pixmap;
	Pixmap x11_pixmap;

	int numReturned;
	int result;
	int dummyAttributes_1_3[] = {
		GLX_DRAWABLE_TYPE, GLX_PIXMAP_BIT,
	        GLX_RENDER_TYPE,   GLX_RGBA_BIT,
		None
	};

	fbConfigs = glXChooseFBConfig( dpy, DefaultScreen(dpy),
					 dummyAttributes_1_3,
					&numReturned );

	vInfo = glXGetVisualFromFBConfig( dpy, fbConfigs[0] );

	dump_vinfo(vInfo);

	x11_pixmap = XCreatePixmap( dpy, DefaultRootWindow(dpy) , 10, 10, 8 );

	if (hybrid)
		glx_pixmap = glXCreateGLXPixmap( dpy, vInfo, x11_pixmap ); //GLX 1.2
	else
		glx_pixmap = glXCreatePixmap( dpy, fbConfigs[0], x11_pixmap, NULL ); // GLX 1.3

        context = glXCreateNewContext( dpy, fbConfigs[0], GLX_RGBA_TYPE, NULL, True );

	glXMakeContextCurrent( dpy, glx_pixmap, glx_pixmap, context );

	return 1;
}

int glx_pixmap_dummy()
{
	Display	*dpy;
	int major;
	int minor;

	dpy = XOpenDisplay( NULL );
	if ( dpy == NULL ) {
		printf( "Unable to open a connection to the X server\n" );
	return 0;
	}

	/*
	On some systems, the GLX library will report that it is version
	1.2, but some 1.3 functions will be implemented, and, furthermore,
	some 1.2 functions won't work right, while the 1.3 functions will.
	The 1.3 will however generate a MESA warning at runtime.

	To workaround this, detect the situation and use 'hybrid' mix of
	1.2 and 1.3 as needed.
	*/
	glXQueryVersion(dpy, &major, &minor);

	if (minor<=2)
		if (glXCreatePixmap!=NULL) // 1.3 function exists
			return glx_1_3_pixmap_dummy(dpy,True);
		else
			return glx_1_2_pixmap_dummy(dpy);
	else if (minor>=3)
		return glx_1_3_pixmap_dummy(dpy,False);
}

