
CPPFLAGS+=-Wall -Wextra -Wno-implicit-fallthrough
CFLAGS+=-ggdb -O2 -fPIC
SHELL=bash


VERSION=1
SOVERSION=1

DESTDIR?=/usr
LIBDIR?=$(DESTDIR)/lib
INCLUDEDIR?=$(DESTDIR)/include
MANDIR?=$(DESTDIR)/share/man

SONAME=libmurmurhash.so.$(SOVERSION)
OBJECTS=PMurHash.o murmurhash.o

.DEFAULT: all
.PHONY: all clean check format
all: libmurmurhash.a $(SONAME) mmh

## build the library
PMurHash.o: PMurHash.c PMurHash.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ PMurHash.c

murmurhash.o: murmurhash.c murmurhash.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ murmurhash.c

libmurmurhash.a: $(OBJECTS)
	$(AR) -qs $@ $^

$(SONAME): libmurmurhash.a
	$(CC) -Wl,-soname,$(SONAME) -shared -o $@ $^

## checks
mmh.o: test/mmh.c
	$(CC) -I. $(CPPFLAGS) $(CFLAGS) -c -o $@ $^

MurmurHash3.o: test/MurmurHash3.c test/MurmurHash3.h
	$(CC) -I. $(CPPFLAGS) $(CFLAGS) -c -o $@ test/MurmurHash3.c

mmh: mmh.o libmurmurhash.a
	$(CC) $(CFLAGS) -o $@ $^ -Wl,--whole-archive -L. -lmurmurhash -Wl,--no-whole-archive

mmh_r: mmh.o MurmurHash3.o
	$(CC) $(CFLAGS) -o $@ $^

check: mmh mmh_r
	diff test/almostempty.hash <(./mmh test/almostempty)

format:
	clang-format -i *.c *.h test/*.c test/*.h

clean:
	$(RM) *.o *.a *.so *.so.* mmh mmh_r
	$(RM) test/*.o
