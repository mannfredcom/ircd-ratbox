
AUTOMAKE_OPTIONS = foreign

CPPFLAGS = -I. -I../include @OpenSSL_CFLAGS@ @GNUTLS_CFLAGS@ @MBEDTLS_CFLAGS@
CFLAGS = @WARNFLAGS@ @EXTRA_WARNINGS@ @CFLAGS@ 
BUILT_SOURCES = version.c

version.c: version.c.SH ../CREDITS
	$(SHELL) ./version.c.SH
	$(CP) version.c version.c.last

libratbox_la_SOURCES =			\
	unix.c				\
	win32.c				\
	crypt.c				\
	commio.c			\
	openssl.c			\
	gnutls.c			\
	nossl.c				\
	event.c				\
	ratbox_lib.c			\
	rb_memory.c			\
	linebuf.c			\
	snprintf.c			\
	rb_tools.c			\
	helper.c			\
	devpoll.c			\
	epoll.c				\
	poll.c				\
	ports.c				\
	select.c			\
	kqueue.c			\
	rawbuf.c			\
	patricia.c			\
	arc4random.c			\
	mbedtls.c			\
	version.c
	

libratbox_la_LDFLAGS = @CRYPT_LIB@ @OpenSSL_LIBS@ @GNUTLS_LIBS@ @MBEDTLS_LIBS@ @LIBJEMALLOC@ @LIBTCMALLOC@  -avoid-version -no-undefined -export-symbols export-syms.txt
libratbox_la_LIBADD = @CRYPT_LIB@ @OpenSSL_LIBS@ @GNUTLS_LIBS@ @MBEDTLS_LIBS@ @LIBJEMALLOC@ @LIBTCMALLOC@
lib_LTLIBRARIES = libratbox.la

