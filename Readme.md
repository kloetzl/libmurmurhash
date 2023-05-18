# LibMurmurHash

MurmurHash is one of the most commonly used non-cryptographic hash functions. This project bundles it into a shared library to avoid code duplication. Further it provides compatibility even for big-endian systems.

## Compiling and Installing

libmurmurhash uses the autotools as the build system. When building from a release tarball they are optional as is the first step of the following.

    autoreconf -fi  # optional
    ./configure
    make
    make check
    make install

## API

LibMurmurHash provides the three variants of MurmurHash3 from the [original source](https://github.com/aappleby/smhasher). The first parameter specifies the beginning of the data to be hashed in memory. The second parameter gives the number of bytes to be hashed. The third parameter can be used to compute an alternative hash on the same data. Pass the location of the hash via the output parameter.

```C
#include <murmurhash.h>

void lmmh_x86_32(const void *addr, unsigned int len, uint32_t seed, uint32_t out[1]);
void lmmh_x86_128(const void *addr, unsigned int len, uint32_t seed, uint32_t out[4]);
void lmmh_x64_128(const void *addr, unsigned int len, uint32_t seed, uint64_t out[2]);
```

For compatibility the original API is still supported but *deprecated*. It has two problems; The given length could be negative; The out parameter has some peculiar alignment requirements.

```C
void MurmurHash3_x86_32(const void *data, int len, uint32_t seed, void *out);
void MurmurHash3_x86_128(const void *data, int len, uint32_t seed, void *out);
void MurmurHash3_x64_128(const void *data, int len, uint32_t seed, void *out);
```

## License

No license—all code is public domain. I appreciate credit, though.

## Contact

Please file a bug on GitHub <github.com/kloetzl/libmurmurhash> or send me a mail <fabian-libmurmurhash@kloetzl.info>.
