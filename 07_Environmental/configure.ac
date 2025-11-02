#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([Key dumper], [0.01], [fomindmitriyfoma@mail.ru])
AM_INIT_AUTOMAKE([foreign subdir-objects])

AC_CONFIG_SRCDIR([src/rhasher.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([rhash],[rhash_msg],[],[AC_MSG_ERROR([RHASH IS NEEDED])])
AC_CHECK_LIB([readline],[readline])

# Checks for header files.
AC_CHECK_HEADERS([rhash.h])
AC_CHECK_HEADER([readline/readline.h])
AC_CHECK_HEADER([readline/history.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T([size_t])

# Checks for library functions.
AC_CHECK_FUNCS([strerror])

# Optional clues
AC_ARG_ENABLE([readline],
    AS_HELP_STRING([--enable-readline], [Use ARROWS to edit your input]),
    AC_DEFINE(READLINE,[],[Use lib readline])
  )

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
