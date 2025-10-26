CFLAGS=-Wall -fPIC -O0
CC=cc
GENERATES=move evil_unlink.so
TRASH=*.in *.out *.out.1

all: move evil_unlink.so

move: move.c
	$(CC) $(CFLAGS) move.c -o $@

evil_unlink.so: evil_unlink.c
	$(CC) -shared $(CFLAGS) -o $@ $<

tests_common: move
	python3 test.py test_common

test_ok: move
	python3 test.py test_ok

test_protect: move evil_unlink.so
	python3 test.py test_protect

test: test_ok test_protect tests_common

clean:
	rm -f $(GENERATES) $(TRASH)
