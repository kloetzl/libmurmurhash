lib_LTLIBRARIES  = libmurmurhash.la

libmurmurhashdir=$(includedir)
libmurmurhash_HEADERS= murmurhash.h
libmurmurhash_la_SOURCES = PMurHash.c murmurhash.c PMurHash.h
libmurmurhash_la_LDFLAGS = -version-info @LIB_VERSION@

libmurmurhash_la_CPPFLAGS = $(INCLUDES)


pkgconfigdir=$(libdir)/pkgconfig
pkgconfig_DATA = libmurmurhash.pc


## checks
mmh.o: test/mmh.c
	$(CC) -I. $(CPPFLAGS) $(CFLAGS) -c -o $@ $^

mmh_old.o: test/mmh_old.c
	$(CC) -I. $(CPPFLAGS) $(CFLAGS) -c -o $@ $^

MurmurHash3.o: test/MurmurHash3.c test/MurmurHash3.h
	$(CC) -I. $(CPPFLAGS) $(CFLAGS) -c -o $@ test/MurmurHash3.c

# new interface, static linking
mmh: mmh.o libmurmurhash.la
	$(CC) $(CFLAGS) -o $@ mmh.o .libs/libmurmurhash.a # -L.libs -lmurmurhash

# new interface, dynamic linking
mmh_dyn: test/mmh.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^ -lmurmurhash

# old interface, static linking
mmh_old: mmh_old.o libmurmurhash.la
	$(CC) $(CFLAGS) -o $@ mmh_old.o .libs/libmurmurhash.a # -L.libs -lmurmurhash

# old interface, reference implementation
mmh_ref: mmh_old.o MurmurHash3.o
	$(CC) $(CFLAGS) -o $@ $^

check: mmh mmh_old
	./mmh test/almostempty | diff test/almostempty.hash -
	./mmh_old test/almostempty | diff test/almostempty.hash -

# Only execute this on x86 or x86_64
check-ref: mmh_ref
	./mmh_ref test/almostempty | diff test/almostempty.hash -

# should be checked after installation!
check-dynamic: mmh_dyn
	./mmh_dyn test/almostempty | diff test/almostempty.hash -

## misc

format:
	clang-format -i *.c *.h test/*.c test/*.h

