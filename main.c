/*

 This is a minimal example of how to use an OpenGL Framebuffer Object
 (sometimes called an FBO) to draw OpenGL commands to an 'offscreen buffer'
 and save the results to a TARGA (.tga) image file.

 Question: Why not just use OSMesa?
 Answer: I don't understand OSMesa.

 Question: How is it 'offscreen' if you have to create a window first?
 Answer: The FBO functions cannot be called until an OpenGL context is created.
  How do you create an OpenGL Context? There is no standard way.

  You can create a window using a wrapper like SDL or GLU - this automatically
  creates a Context.

  If you want an 'offscreen' context, you have to use something like
  Apple's NSOpenGLContext or GLXCreateNewContext.

  This program shows several different ways. The GLX_PIXMAP_DUMMY is
  currently the only version that works without creating any window.

  You can switch between the context-creation methods by editing the Makefile

 Question: My OpenGL isn't working?
 Answer: This command line will use a 'software' renderer, which might help,
  but it also might make the thing crash.

 LIBGL_ALWAYS_SOFTWARE=1 ./fbo

 Question: So... i see this GLEW stuff in there...
 Answer: Glew is completely unnecessary for this code. But just in case
  you find yourself in a situation where GLEW is necessary, I have included
  a GLEW_UGH ifdef to show how it can be accomplished.

See Also


http://en.wikibooks.org/wiki/OpenGL_Programming - hello world
http://glprogramming.com/red/chapter10.html - hello triangles
http://duriansoftware.com/joe/An-intro-to-modern-OpenGL.-Chapter-2:-Hello-World:-The-Slideshow.html - hello glut
http://duriansoftware.com/joe/An-intro-to-modern-OpenGL.-Chapter-2:-Hello-World:-The-Slideshow.html - hello SDL/OpenGL
http://www.gamedev.net/topic/573940-glx-why-does-glxcreatewindow-exist/ GLX hello
http://www.opengl.org/sdk/docs/man/xhtml/glXIntro.xml GLX Hello world

http://www.opengl.org/wiki/GL_EXT_framebuffer_object - framebuffer info
http://www.opengl.org/wiki/Framebuffer_Object - more framebuffer info
http://www.opengl.org/wiki/Image_Format - description of RGBA 8, etc

http://www.songho.ca/opengl/gl_fbo.html Song Ho Ahn's Framebuffer example code
http://www.mesa3d.org/ 'osdemo' from 'Mesa Demos'

http://www.mesa3d.org/brianp/sig97/offscrn.htm - explains offscreen in Windows and Unix before FBO was invented
http://www.gamedev.net/topic/552607-conflict-between-glew-and-sdl/ - glew + sdl
http://www.gamedev.net/topic/487783-solved-glcheckframebufferstatusext-returns-0/ - error checking FBO setup

Offscreen Mesa info:

http://www.mesa3d.org/osmesa.html
http://www.mesa3d.org/brianp/sig97/offscrn.htm

ARB vs EXT vs core info:

http://www.opengl.org/wiki/Framebuffer_Object
http://stackoverflow.com/questions/6912988/glgenframebuffers-or-glgenframebuffersex
http://www.devmaster.net/forums/showthread.php?t=10967

Glx 1.2 vs Glx 1.3:

http://publib.boulder.ibm.com/infocenter/pseries/v5r3/index.jsp?topic=/com.ibm.aix.opengl/doc/openglrf/HowRenderXdraw.htm
https://bugs.launchpad.net/ubuntu/+source/compiz/+bug/433488
http://www.opengl.org/sdk/docs/man/xhtml/glXIntro.xml
http://www.opengl.org/discussion_boards/ubbthreads.php?ubb=showflat&Number=299750 # GlxCreateWindow / Pbuffers
http://glprogramming.com/blue/ch07.html # GLX 1.2 initialization

OSMESA:

http://www.gnashdev.org/?q=node/46

XLIB:

http://tronche.com/gui/x/xlib-tutorial/2nd-program-anatomy.html - XLIB basics

*/

#ifdef GLEW_UGH
#define NO_SDL_GLEXT
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <stdio.h>

#if defined(SDL_DUMMY)
#include <SDL/SDL.h>
#elif defined(GLUT_DUMMY)
#include <GL/glut.h>
#elif defined(GLX_DUMMY)
extern int glx_dummy();
#elif defined(GLX_PIXMAP_DUMMY)
extern int glx_pixmap_dummy();
#else
#error "need SDL_DUMMY or GLUT_DUMMY or GLX_DUMMY or GLX_PIXMAP_DUMMY defined"
#endif

const int IMAGE_WIDTH = 256;
const int IMAGE_HEIGHT = 256;
int angle = 0;

GLuint fboId;
GLuint textureId;
GLuint renderbufferId;

void write_targa(char *filename, GLubyte *pixels, int width, int height)
{
	// based on mesademos, osmesa, public domain originally by Brian Paul
	FILE *f = fopen( filename, "w" );
	int y;
	if (f) {
		GLubyte header[] = {
			00,00,02, 00,00,00, 00,00,00, 00,00,00,
			0xff & width, 0xff & width >> 8,
			0xff & height, 0xff & height >> 8,
			32, 0x20 }; // next-to-last = bit depth
		fwrite( header, sizeof(header), 1, f);
		for (y=height-1; y>=0; y--)
			fwrite( pixels + y*width*4, 4, width, f);
		fclose(f);
	}
}

void draw_scene()
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);

	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
	glRotatef(angle, 0,0,1);
	glBegin(GL_TRIANGLES);
	glColor3f( 1, 0, 0);
	glVertex3f( 0, 0, 0);
	glVertex3f( 1, 0, 0);
	glVertex3f( 0, 1, 0);
	glEnd();
	glPopMatrix();

	angle += 15;

	GLubyte pixels[IMAGE_WIDTH*IMAGE_HEIGHT*4];
	glReadPixels(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, GL_BGRA, GL_UNSIGNED_BYTE, pixels);
	char fname[30];
	snprintf(fname,30,"test%04i.tga",angle);
	printf("writing %s\n",fname);
	write_targa(fname,pixels,IMAGE_WIDTH,IMAGE_HEIGHT);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void init_dummy_window(int argc, char **argv)
{
#ifdef GLUT_DUMMY
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_ALPHA);
	glutInitWindowSize(10, 10);
	glutCreateWindow("fbotest");
#endif
#ifdef SDL_DUMMY
	SDL_Init(SDL_INIT_VIDEO);
	SDL_SetVideoMode(10,10,32,SDL_OPENGL);
#endif
#ifdef GLX_DUMMY
	glx_dummy();
#endif
#ifdef GLX_PIXMAP_DUMMY
	glx_pixmap_dummy();
#endif
}


int check_fbo_status()
{
	/* lots of things can go wrong when attaching framebuffers.
	Song Ho's page is good: http://www.songho.ca/opengl/gl_fbo.html
	This code is based on user V-man code from
	http://www.opengl.org/wiki/GL_EXT_framebuffer_multisample
	*/
	GLenum status;
	int result = -1;
	status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if (glGetError() != GL_NO_ERROR)
		printf("OpenGL Error %i\n",glGetError());
	if (status == GL_FRAMEBUFFER_COMPLETE_EXT) {
		printf("OpenGL Framebuffer object status is OK\n");
		result = 0;
	} else {
		printf("OpenGL error setting up Framebuffer Object: ");
	}

	if (status == GL_FRAMEBUFFER_UNSUPPORTED_EXT)
		printf("GL_FRAMEBUFFER_UNSUPPORTED_EXT\n");
	else if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT)
		printf("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT\n");
	else if (status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT)
		printf("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT\n");
	else if (status == GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT)
		printf("GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT - width / height problems?\n");
	else if (status == GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT)
		printf("GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT\n");
	else if (status == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT)
		printf("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT\n");
	else if (status == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT)
		printf("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT\n");
	else if (status == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT)
		printf("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT\n");
	else if (result !=0 ) {
		printf("\n Unknown Code: glCheckFramebufferStatusEXT returned %i\n",status);
	}
	return result;
}

void check_glerror()
{
	if (glGetError())
		printf( "OpenGL Error status: %i\n", glGetError());
}

int main(int argc, char **argv)
{
	init_dummy_window(argc, argv);

#ifdef GLEW_UGH
	// must come after openGL context init (done by dummy window)
	glewInit();
	printf("ARB FBO: %i\n",glewIsSupported("GL_ARB_framebuffer_object"));
	printf("EXT FBO: %i\n",glewIsSupported("GL_EXT_framebuffer_object"));
#endif

	// create framebuffer object
	glGenFramebuffersEXT(1, &fboId);
	check_glerror();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);
	check_glerror();

	// create renderbuffer object
	glGenRenderbuffersEXT(1, &renderbufferId);
	check_glerror();
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderbufferId);
	check_glerror();
	glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_RGBA, IMAGE_WIDTH, IMAGE_HEIGHT );
	check_glerror();

	// attach renderbuffer to framebuffer at 'color attachment point'
	glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, renderbufferId );
	check_glerror();

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	check_glerror();
	if (check_fbo_status() <0 ) return -1;

	glViewport(0,0,IMAGE_WIDTH,IMAGE_HEIGHT);
	int i;
	for (i=0;i<4;i++) draw_scene();

	glDeleteRenderbuffersEXT(1, &renderbufferId);
	glDeleteFramebuffersEXT(1, &fboId);

	return 0;
}


