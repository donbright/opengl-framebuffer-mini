CC   = gcc
CFLAGS = -g
# LIBS =  -lglut -lGLU -lGL
LIBS =  -lSDL -lGL
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before fbo all-after 

clean: clean-custom
	${RM} main.o fbo

fbo: main.o
	$(CC) -o fbo $(LIBS) main.o

main.o: main.c
	$(CC) -c main.c -o main.o $(CFLAGS)
