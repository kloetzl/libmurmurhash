
CPPFLAGS+=-Wall -Wextra -Wno-implicit-fallthrough
CFLAGS+=-ggdb -O2 -fPIC
SHELL=bash


VERSION=1.1
SOVERSION=1
PROJECT_VERSION=libmurmurhash-$(VERSION)
TARBALL=$(PROJECT_VERSION).tar.gz

DESTDIR?=/usr
LIBDIR?=$(DESTDIR)/lib
INCLUDEDIR?=$(DESTDIR)/include
MANDIR?=$(DESTDIR)/share/man
INSTALL?=install

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
	$(CC) -Wl,-soname,$(SONAME) -shared -o $@ -Wl,--whole-archive -L. -lmurmurhash -Wl,--no-whole-archive

## installation
.PHONY: install install-dev install-lib uninstall dist distcheck

install: install-dev install-lib

install-dev:
	$(INSTALL) -D -m0644 -t "$(INCLUDEDIR)" murmurhash.h
	$(INSTALL) -D -m0644 -t "$(MANDIR)/man3" murmurhash.3
	ln -f -r -s "$(MANDIR)/man3/murmurhash.3" "$(MANDIR)/man3/MurmurHash3_x86_32.3"
	ln -f -r -s "$(MANDIR)/man3/murmurhash.3" "$(MANDIR)/man3/MurmurHash3_x86_128.3"
	ln -f -r -s "$(MANDIR)/man3/murmurhash.3" "$(MANDIR)/man3/MurmurHash3_x64_128.3"

install-lib: $(SONAME)
	$(INSTALL) -D -t "$(LIBDIR)" $(SONAME)
	ln -f -r -s "$(LIBDIR)/$(SONAME)" "$(LIBDIR)/libmurmurhash.so"

uninstall:
	$(RM) "$(INCLUDEDIR)/murmurhash.h"
	$(RM) "$(LIBDIR)/libmurmurhash.so"
	$(RM) "$(LIBDIR)/$(SONAME)"
	$(RM) "$(MANDIR)/man3/murmurhash.3"
	$(RM) "$(MANDIR)/man3/MurmurHash3_*"

$(TARBALL):
	mkdir -p "$(PROJECT_VERSION)"/test
	cp Makefile Readme.md NOLICENSE "$(PROJECT_VERSION)"
	cp *.h *.c "$(PROJECT_VERSION)"
	cp test/*.c test/*.h "$(PROJECT_VERSION)/test"
	cp test/almostempty test/almostempty.hash "$(PROJECT_VERSION)/test"
	tar -ca -f $@ "$(PROJECT_VERSION)"
	$(RM) -r "$(PROJECT_VERSION)"

dist: $(TARBALL)

distcheck: dist
	tar -xzvf "$(TARBALL)"
	$(MAKE) -C "$(PROJECT_VERSION)"
	$(MAKE) -C "$(PROJECT_VERSION)" check
	$(RM) -r "$(PROJECT_VERSION)"

## checks
mmh.o: test/mmh.c
	$(CC) -I. $(CPPFLAGS) $(CFLAGS) -c -o $@ $^

mmh_old.o: test/mmh_old.c
	$(CC) -I. $(CPPFLAGS) $(CFLAGS) -c -o $@ $^

MurmurHash3.o: test/MurmurHash3.c test/MurmurHash3.h
	$(CC) -I. $(CPPFLAGS) $(CFLAGS) -c -o $@ test/MurmurHash3.c

# new interface, static linking
mmh: mmh.o libmurmurhash.a
	$(CC) $(CFLAGS) -o $@ $^ -L. -lmurmurhash

# new interface, dynamic linking
mmh_dyn: test/mmh.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^ -lmurmurhash

# old interface, static linking
mmh_old: mmh_old.o libmurmurhash.a
	$(CC) $(CFLAGS) -o $@ $^ -L. -lmurmurhash

# old interface, reference implementation
mmh_ref: mmh_old.o MurmurHash3.o
	$(CC) $(CFLAGS) -o $@ $^

check: mmh mmh_old mmh_ref
	diff test/almostempty.hash <(./mmh test/almostempty)
	diff test/almostempty.hash <(./mmh_old test/almostempty)
	diff test/almostempty.hash <(./mmh_ref test/almostempty)

# should be checked after installation!
check-dynamic: mmh_dyn
	diff test/almostempty.hash <(./mmh_dyn test/almostempty)


## misc

format:
	clang-format -i *.c *.h test/*.c test/*.h

clean:
	$(RM) *.o *.a *.so *.so.* mmh mmh_*
	$(RM) test/*.o *.tar.gz
	$(RM) -r "$(PROJECT_VERSION)"
