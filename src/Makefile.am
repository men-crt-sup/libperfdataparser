bin_PROGRAMS = perfdata2json
perfdata2json_SOURCES = perfdata2json.c
perfdata2json_LDADD = libperfdataparser.la

lib_LTLIBRARIES = libperfdataparser.la
libperfdataparser_la_SOURCES = libperfdataparser.c
include_HEADERS = libperfdataparser.h

AM_CFLAGS =-Wall -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wstrict-prototypes -Wmissing-prototypes -Wnested-externs -fPIC