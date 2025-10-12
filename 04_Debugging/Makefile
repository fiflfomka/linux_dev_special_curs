CFLAGS=-O0 -g
GENERATES=*.o range *.out

all: range

clean:
	rm -f $(GENERATES)

test_%: range
	gdb ./range -x gdb_$@.txt | grep ANSWER > $@.out
	cmp $@.out correct_out_$@.txt
	echo ret code is $$? where '0' is success and 'OTHER' is fail "\n"

test: test_A test_B
