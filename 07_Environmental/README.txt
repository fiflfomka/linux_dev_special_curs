# компиляция
autoreconf
automake --add-missing
./configure

# тесты
make
make test-suite.log


autoreconf ; automake --add-missing ; ./configure ; make ; rm -f test-suite.log ; make test-suite.log ;
