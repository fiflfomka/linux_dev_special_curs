CC=cc
EXE=Shower
SRC=Shower.c
LDLIBS=-lncursesw
CFLAGS=-Wall -O0

all: ${EXE}

run:
	./${EXE} text.txt

valgrind:
	valgrind ./${EXE} text.txt

clean:
	rm -f *.o ${EXE}
