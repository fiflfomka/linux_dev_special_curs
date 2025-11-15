CFLAGC= -Wall -O0 -g
AM_CPPFLAGS= -D'LOCALEDIR="$(localedir)"'
SUBDIRS=po
bin_PROGRAMS=bin_searcher
pkgdata_DATA=ru/LC_MESSAGES/bin_searcher.mo

ru/LC_MESSAGES/bin_searcher.mo: po/ru.po
	mkdir --parents `dirname $@`
	msgfmt $< -o $@

po/ru.po:	po/bin_searcher.pot
	msgmerge -U $@ $<

po/bin_searcher.pot:	bin_searcher.c
	cd po; intltool-update ru
