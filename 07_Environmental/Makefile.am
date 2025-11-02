CFLAGS = -Wall -O0 -g

bin_PROGRAMS=rhasher

rhasher_SOURCES=src/rhasher.c

TESTS= test_script_sha1.sh test_script_md5.sh

test_script_sha1.sh:
	echo 'echo 123 > BIN_FILE' > $@
	echo 'sha1sum BIN_FILE | cut -f1 -d " " > $@.correct' >> $@
	echo 'echo -n "SHA1 BIN_FILE" | ./rhasher > $@.rhasher' >> $@
	echo 'cmp $@.correct $@.rhasher' >> $@
	chmod +x $@

test_script_md5.sh:
	echo 'echo -n "123" | md5sum | cut -f1 -d " " > $@.correct' >> $@
	echo 'echo -n "MD5 \"123" | ./rhasher > $@.rhasher' >> $@
	echo 'cmp $@.correct $@.rhasher' >> $@
	chmod +x $@

clean:
	git clean -xdf
	rm -rf ${GENERATES}
