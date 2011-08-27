/* minimal example

 using OpenGL Framebuffer object to write to an 'offscreen buffer',
 and save the results to a TARGA (.tga) image file.

 GLUT or SDL are required only to create a 'dummy window'. you can
 experiment with using one or the other by uncommenting the #includes,
 the init_dummy_window() function, and the Makefile

 Question: How is it 'offscreen' if you have to create a window?
 Answer: OpenGL pretty much requires a window to init itself. After that,
  the window is not used. That's why it's called a "dummy window".

 Question: How can I use it if I don't have an OpenGL graphics card? 
 Answer: This command line will use a 'software' renderer:

 LIBGL_ALWAYS_SOFTWARE=1 ./fbo

See Also

http://en.wikibooks.org/wiki/OpenGL_Programming - hello world
http://glprogramming.com/red/chapter10.html - hello triangles

http://www.opengl.org/wiki/GL_EXT_framebuffer_object - framebuffer info
http://www.opengl.org/wiki/Framebuffer_Object - more framebuffer info
http://www.opengl.org/wiki/Image_Format - description of RGBA 8, etc

http://www.songho.ca/opengl/gl_fbo.html Song Ho's Framebuffer example code
http://www.mesa3d.org/ 'osdemo' from 'Mesa Demos'

*/

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include <SDL/SDL.h>
//#include <GL/glut.h>

#include <stdio.h>

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
	glReadPixels(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	char fname[30];
	snprintf(fname,30,"test%04i.tga",angle);
	printf("writing %s\n",fname);
	write_targa(fname,pixels,IMAGE_WIDTH,IMAGE_HEIGHT);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void init_dummy_window()
{
/*
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_ALPHA);
	glutInitWindowSize(10, 10);
	glutCreateWindow("fbotest");
*/
	SDL_Init(SDL_INIT_VIDEO);
	SDL_SetVideoMode(10,10,32,SDL_OPENGL);
}


int main(int argc, char **argv)
{
	init_dummy_window();

	// create framebuffer object
	glGenFramebuffersEXT(1, &fboId);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboId);

	// create renderbuffer object
	glGenRenderbuffers(1, &renderbufferId);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderbufferId);
	glRenderbufferStorage( GL_RENDERBUFFER_EXT, GL_RGBA8, IMAGE_WIDTH, IMAGE_HEIGHT );

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


