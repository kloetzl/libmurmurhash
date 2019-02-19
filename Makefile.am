# AutoMake Me
dist_noinst_DATA = NOLICENSE Readme.md

lib_LTLIBRARIES  = libmurmurhash.la

libmurmurhashdir=$(includedir)
libmurmurhash_HEADERS= murmurhash.h
libmurmurhash_la_SOURCES = PMurHash.c murmurhash.c PMurHash.h
libmurmurhash_la_LDFLAGS = -version-info @LIB_VERSION@

libmurmurhash_la_CPPFLAGS = $(INCLUDES)


pkgconfigdir=$(libdir)/pkgconfig
pkgconfig_DATA = libmurmurhash.pc


## checks
check_PROGRAMS=mmh mmh_old

# new interface, static linking
mmh_SOURCES = test/mmh.c
mmh_LDADD = libmurmurhash.la

# new interface, dynamic linking
mmh_dyn_SOURCES = test/mmh.c
mmh_dyn_LDADD = -lmurmurhash

# old interface, static linking
mmh_old_SOURCES = test/mmh_old.c
mmh_old_LDADD = libmurmurhash.la

# old interface, reference implementation
mmh_ref_SOURCES = test/mmh_old.c test/MurmurHash3.c test/MurmurHash3.h

dist_noinst_DATA += test/almostempty test/almostempty.hash

check: mmh mmh_old
	./mmh $(top_srcdir)/test/almostempty | diff $(top_srcdir)/test/almostempty.hash -
	./mmh_old $(top_srcdir)/test/almostempty | diff $(top_srcdir)/test/almostempty.hash -

# Only execute this on x86 or x86_64
check-ref: mmh_ref
	./mmh_ref $(top_srcdir)/test/almostempty | diff $(top_srcdir)/test/almostempty.hash -

# should be checked after installation!
check-dynamic: mmh_dyn
	./mmh_dyn $(top_srcdir)/test/almostempty | diff $(top_srcdir)/test/almostempty.hash -

## misc
format:
	clang-format -i *.c *.h test/*.c test/*.h

