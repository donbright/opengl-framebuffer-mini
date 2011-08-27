CC   = gcc
RM = rm -f
OBJS = main.o

# for SDL use this
CFLAGS = -g -DSDL_DUMMY
LIBS =  -lSDL -lGL

# for glut use this
#CFLAGS = -g -DGLUT_DUMMY
#LIBS =  -lglut -lGL

# for glx use this
#CFLAGS = -g -DGLX_DUMMY
#LIBS =  -lX11 -lGL
#OBJS += glx_dummy.o

.PHONY: all all-before all-after clean clean-custom

all: all-before fbo all-after

clean: clean-custom
	${RM} main.o fbo

fbo: $(OBJS)
	$(CC) -o fbo $(LIBS) $(OBJS)

glx_dummy.o: 
	$(CC) -c glx_dummy.c -o glx_dummy.o $(CFLAGS)
	
main.o: main.c
	$(CC) -c main.c -o main.o $(CFLAGS)

run: fbo
	rm -f ./*.tga && ./fbo && eog ./*.tga
