CC   = gcc
RM = rm -f
OBJS = main.o

# for SDL uncomment this
# CFLAGS = -g -DSDL_DUMMY
# LIBS =  -lSDL -lGL

# for glut uncomment this
#CFLAGS = -g -DGLUT_DUMMY
#LIBS =  -lglut -lGL

# for glx using a tiny window uncomment this
CFLAGS = -g -DGLX_DUMMY
LIBS =  -lX11 -lGL
OBJS += glx_dummy.o

# for glx fully offscreen, using GLXpixmap, uncomment this
#CFLAGS = -g -DGLX_PIXMAP_DUMMY
#LIBS =  -lX11 -lGL
#OBJS += glx_pixmap_dummy.o

# yet another example: for SDL and GLEW together, uncomment this
# CFLAGS = -g -DSDL_DUMMY -DGLEW_UGH
# LIBS =  -lGL -lGLEW -lSDL

.PHONY: all all-before all-after clean clean-custom

all: all-before fbo all-after

clean: clean-custom
	${RM} main.o fbo *.o *.tga

fbo: $(OBJS)
	$(CC) -o fbo $(LIBS) $(OBJS)

glx_dummy.o: glx_dummy.c 
	$(CC) -c glx_dummy.c -o glx_dummy.o $(CFLAGS)

glx_pixmap_dummy.o: glx_pixmap_dummy.c 
	$(CC) -c glx_pixmap_dummy.c -o glx_pixmap_dummy.o $(CFLAGS)
	
main.o: main.c
	$(CC) -c main.c -o main.o $(CFLAGS)

run: fbo
	rm -f ./*.tga && ./fbo && eog ./*.tga
