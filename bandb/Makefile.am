AUTOMAKE_OPTIONS = foreign subdir-objects

if BUILD_SQLITE
SUBDIRS = sqlite3
@sqlite3_CFLAGS@="-Isqlite3"
endif

libexec_PROGRAMS = bandb 
bin_PROGRAMS = bantool
AM_CFLAGS=$(WARNFLAGS)

AM_CPPFLAGS = -I../include -I../libratbox/include @sqlite3_CFLAGS@


bandb_SOURCES = bandb.c rsdb_sqlite3.c 

bandb_LDADD = ../libratbox/src/libratbox.la @sqlite3_LIBS@

bantool_SOURCES = bantool.c rsdb_sqlite3.c 
bantool_LDADD = ../libratbox/src/libratbox.la @sqlite3_LIBS@

