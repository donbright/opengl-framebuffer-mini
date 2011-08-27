CC   = gcc
RM = rm -f

# for SDL use this
#CFLAGS = -g -DSDL_DUMMY
#LIBS =  -lSDL -lGL

# for glut use this
CFLAGS = -g -DGLUT_DUMMY
LIBS =  -lglut -lGL


.PHONY: all all-before all-after clean clean-custom

all: all-before fbo all-after 

clean: clean-custom
	${RM} main.o fbo

fbo: main.o
	$(CC) -o fbo $(LIBS) main.o

main.o: main.c
	$(CC) -c main.c -o main.o $(CFLAGS)

run: fbo
	rm -f ./*.tga && ./fbo && eog ./*.tga
