# LIBMURMURHASH

MurmurHash is one of the most commonly used non-cryptographic hash functions. This project bundles it into a shared library to avoid code duplication. Further it provides compatibility even for big-endian systems.

## Compiling and Installing

libmurmurhash has no prerequisites other than a reasonably up-to-date UNIX system.

    make
    make check
    make DESTDIR=/usr/local install

## License

No license—all code is public domain. I appreciate credit, though.

## Contact

Please file a bug on GitHub <github.com/kloetzl/libmurmurhash> or send me a mail <kloetzl@evolbio.mpg.de>.
