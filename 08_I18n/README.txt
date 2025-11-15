# Сборка
autoreconf -fisv
./configure --localedir=`pwd`
make

# для первого запуска УБРАТЬ po/ru.po
# будет сообщение об ошибке
cd po
msginit
cd ..
# вот теперь успех
make


# Запуск
./bin_searcher
LANG=ru_RU.utf8 ./bin_searcher
