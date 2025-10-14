CC=cc
CFLAGS=-Wall
GENERATES=esub
TRASH=*.out

test: esub
	/bin/echo 123123456 | sed -E "s/123/0/" > sed_0.out
	./esub 123 0 123123456 > esub_0.out
	cmp sed_0.out esub_0.out
	echo
	/bin/echo 123\\456 | sed -E "s/123(.)/A\\1/" > sed_1.out
	./esub "123(.)" A\\1 123\\456 > esub_1.out
	cmp sed_1.out esub_1.out
	echo
	/bin/echo 1231456 | sed -E "s/(.).*\\1/\\1A\\1/" > sed_2.out
	./esub "(.).*\\1" \\1A\\1 1231456 > esub_2.out
	cmp sed_2.out esub_2.out

all: esub

clean:
	rm -f $(GENERATES) $(TRASH)
