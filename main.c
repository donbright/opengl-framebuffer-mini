/* minimal example

 using OpenGL Framebuffer object to write to an 'offscreen buffer',
 and save the results to a TARGA (.tga) image file.

 Question: How is it 'offscreen' if you have to create a window?
 Answer: OpenGL pretty much requires a window to init itself. After that,
  the window is not used. That's why it's called a "dummy window".

 Question: How does it create the dummy window?
 Answer: It uses a snippet of GLUT or SDL or GLX
  You can switch between these options by editing the Makefile

 Question: My OpenGL isn't working?
 Answer: This command line will use a 'software' renderer, which might help.

 LIBGL_ALWAYS_SOFTWARE=1 ./fbo

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

http://www.mesa3d.org/brianp/sig97/offscrn.htm - really old stuff you may run into

*/

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
#else
#error "need SDL_DUMMY or GLUT_DUMMY or GLX_DUMMY defined"
#endif

const int IMAGE_WIDTH = 256;
const int IMAGE_HEIGHT = 256;
int angle = 0;

GLuint fboId;
GLuint textureId;
GLuint renderbufferId;

void write_targa(char *filename, GLubyte *pixels, int width, int height)
{
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
}


int main(int argc, char **argv)
{
	init_dummy_window(argc, argv);

	// create framebuffer object
	glGenFramebuffersEXT(1, &fboId);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);

	// create renderbuffer object
	glGenRenderbuffers(1, &renderbufferId);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderbufferId);
	glRenderbufferStorage( GL_RENDERBUFFER_EXT, GL_RGBA, IMAGE_WIDTH, IMAGE_HEIGHT );

	// attach renderbuffer to framebuffer at 'color attachment point'
	glFramebufferRenderbuffer( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, renderbufferId );

	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if (status == GL_FRAMEBUFFER_COMPLETE_EXT ) {
		printf("OpenGL Framebuffer status OK.\n");
	} else {
		printf("OpenGL Framebuffer error - incomplete attachment.\n");
		return -1;
	}

	glViewport(0,0,IMAGE_WIDTH,IMAGE_HEIGHT);
	int i;
	for (i=0;i<4;i++) draw_scene();

	glDeleteRenderbuffersEXT(1, &renderbufferId);
	glDeleteFramebuffersEXT(1, &fboId);

	return 0;
}


