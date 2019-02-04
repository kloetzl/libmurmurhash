# LibMurmurHash

MurmurHash is one of the most commonly used non-cryptographic hash functions. This project bundles it into a shared library to avoid code duplication. Further it provides compatibility even for big-endian systems.

## Compiling and Installing

libmurmurhash has no prerequisites other than a reasonably up-to-date UNIX system.

    make
    make check
    make DESTDIR=/usr/local install

## API

LibMurmurHash provides the three variants of MurmurHash3 from the [original source](https://github.com/aappleby/smhasher). The first parameter specifies the beginning of the data to be hashed in memory. The second parameter gives the number of bytes to be hashed (must be positive). The third parameter can be used to compute an alternate hash on the same data. The output parameter specifies the location where the resulting hash shall be written to. It should be at least 4-byte aligned.

```C
void MurmurHash3_x86_32(const void *data, int len, uint32_t seed, void *out);
void MurmurHash3_x86_128(const void *data, int len, uint32_t seed, void *out);
void MurmurHash3_x64_128(const void *data, int len, uint32_t seed, void *out);
```

## License

No license—all code is public domain. I appreciate credit, though.

## Contact

Please file a bug on GitHub <github.com/kloetzl/libmurmurhash> or send me a mail <kloetzl@evolbio.mpg.de>.
