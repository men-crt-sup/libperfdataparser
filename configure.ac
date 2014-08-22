dnl Process this file with autoconf to produce a configure script.
AC_INIT(libperfdataparser, 0.1, s.urbanovski@ac-nancy-metz.fr)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(src/libperfdataparser.c)
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([libperfdataparser], [0.1])

LT_INIT

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

# Checks for libraries.

AC_CHECK_LIB([json-c], [json_object_object_get], [], 	[AC_MSG_FAILURE([Could not find usable JSON-C library])], [])

# Checks for header files.
AC_CHECK_HEADERS([limits.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([strdup strndup strspn])


AC_ARG_ENABLE([debug],
    [  --enable-debug           build with additional debugging code],
    [CFLAGS='-g';AC_DEFINE([DEBUG])])



AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
