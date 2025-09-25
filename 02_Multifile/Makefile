.PHONY: clean test all

GENERATES = prog prog-a prog-so liboutput_static.a liboutput.so
TRASH = *.o *~ o.* test*
CFLAGS = -Wall -fPIC

# ALL
all: prog prog-a prog-so

# POINT 0
# generate .obj files
fun.o prog.o: outlib.h

%.o: %.c
	$(CC) $< -c $(CFLAGS) -o $@

# POINT 1
# generate libs
liboutput_static.a: fun.o const.o
	ar -rcs $@ $^

liboutput.so: fun.o const.o
	$(CC) -shared $^ -o $@

# POINT 2
# programs
prog: const.o fun.o prog.o
	$(CC) $^ -o $@

prog-a:	prog.o liboutput_static.a
	$(CC) -L. $< -loutput_static -o $@

prog-so: prog.o liboutput.so
	$(CC) $^ -o $@

# POINT 3
# test
test: test0 test1 test3
	echo
	echo "FILE COMPARATOR: see ret codes (0 if success )"
	cmp test0.out test0.a.out  && echo $$? && \
	cmp test0.out test0.so.out && echo $$? && \
	cmp test1.out test1.a.out  && echo $$? && \
	cmp test1.out test1.so.out && echo $$? && \
	cmp test3.out test3.a.out  && echo $$? && \
	cmp test3.out test3.so.out && echo $$? ;  \
	echo ret code is $$? where '0' is success and 'OTHER' is fail

test0.in:
	echo > test0.in

test1.in:
	echo PARAM1 > test1.in

test3.in:
	echo PARAM1 PARAM2 PARAM3 > test3.in

test%: prog prog-a prog-so test0.in test1.in test3.in
	ARGV=`cat $@.in`
	./prog $$ARGV > $@.out 2>&1
	./prog-a $$ARGV > $@.a.out 2>&1
	LD_LIBRARY_PATH="." ./prog-so $$ARGV > $@.so.out 2>&1

# POINT 4
# CLEAN
clean:
	rm -f $(TRASH) $(GENERATES)
