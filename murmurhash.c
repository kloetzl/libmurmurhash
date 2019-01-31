//-----------------------------------------------------------------------------
// libmurmurhash was written by Fabian Klötzl, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#include "murmurhash.h"
#include "PMurHash.h"
#include <stdio.h>
#include <string.h>

#define weak_alias(old, new)                                                   \
	extern __typeof(old) new __attribute__((weak, alias(#old)))

void lmmh_x86_32(const void *key, int len, MH_UINT32 seed, void *out)
{
	MH_UINT32 hash = PMurHash32(seed, key, len);
	memcpy(out, &hash, sizeof(hash));
}

weak_alias(lmmh_x86_32, MurmurHash3_x86_32);


/*-----------------------------------------------------------------------------
 * Endianess, misalignment capabilities and util macros
 *
 * The following 3 macros are defined in this section. The other macros defined
 * are only needed to help derive these 3.
 *
 * READ_UINT32(x)   Read a little endian unsigned 32-bit int
 * UNALIGNED_SAFE   Defined if READ_UINT32 works on non-word boundaries
 * ROTL32(x,r)      Rotate x left by r bits
 */

/* Convention is to define __BYTE_ORDER == to one of these values */
#if !defined(__BIG_ENDIAN)
#define __BIG_ENDIAN 4321
#endif
#if !defined(__LITTLE_ENDIAN)
#define __LITTLE_ENDIAN 1234
#endif

/* I386 */
#if defined(_M_IX86) || defined(__i386__) || defined(__i386) || defined(i386)
#define __BYTE_ORDER __LITTLE_ENDIAN
#define UNALIGNED_SAFE
#endif

/* gcc 'may' define __LITTLE_ENDIAN__ or __BIG_ENDIAN__ to 1 (Note the trailing
 * __), or even _LITTLE_ENDIAN or _BIG_ENDIAN (Note the single _ prefix) */
#if !defined(__BYTE_ORDER)
#if defined(__LITTLE_ENDIAN__) && __LITTLE_ENDIAN__ == 1 ||                    \
	defined(_LITTLE_ENDIAN) && _LITTLE_ENDIAN == 1
#define __BYTE_ORDER __LITTLE_ENDIAN
#elif defined(__BIG_ENDIAN__) && __BIG_ENDIAN__ == 1 ||                        \
	defined(_BIG_ENDIAN) && _BIG_ENDIAN == 1
#define __BYTE_ORDER __BIG_ENDIAN
#endif
#endif

/* gcc (usually) defines xEL/EB macros for ARM and MIPS endianess */
#if !defined(__BYTE_ORDER)
#if defined(__ARMEL__) || defined(__MIPSEL__)
#define __BYTE_ORDER __LITTLE_ENDIAN
#endif
#if defined(__ARMEB__) || defined(__MIPSEB__)
#define __BYTE_ORDER __BIG_ENDIAN
#endif
#endif

/* Now find best way we can to READ_UINT32 */
#if __BYTE_ORDER == __LITTLE_ENDIAN
/* CPU endian matches murmurhash algorithm, so read 32-bit word directly */
#define READ_UINT32(ptr) (*((uint32_t *)(ptr)))
#elif __BYTE_ORDER == __BIG_ENDIAN
/* TODO: Add additional cases below where a compiler provided bswap32 is
 * available */
#if defined(__GNUC__) &&                                                       \
	(__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3))
#define READ_UINT32(ptr) (__builtin_bswap32(*((uint32_t *)(ptr))))
#else
/* Without a known fast bswap32 we're just as well off doing this */
#define READ_UINT32(ptr) (ptr[0] | ptr[1] << 8 | ptr[2] << 16 | ptr[3] << 24)
#define UNALIGNED_SAFE
#endif
#else
/* Unknown endianess so last resort is to read individual bytes */
#define READ_UINT32(ptr) (ptr[0] | ptr[1] << 8 | ptr[2] << 16 | ptr[3] << 24)

/* Since we're not doing word-reads we can skip the messing about with
 * realignment */
#define UNALIGNED_SAFE
#endif

/* Find best way to ROTL32 */
#if defined(_MSC_VER)
#include <stdlib.h> /* Microsoft put _rotl declaration in here */
#define ROTL32(x, r) _rotl(x, r)
#else
/* gcc recognises this code and generates a rotate instruction for CPUs with one
 */
#define ROTL32(x, r) (((uint32_t)x << r) | ((uint32_t)x >> (32 - r)))
#endif

uint32_t getblock32(const void *addr, size_t offset)
{
	unsigned char data[sizeof(uint32_t)];
	memcpy(&data, addr + offset * sizeof(uint32_t), sizeof(uint32_t));
	return READ_UINT32(data);
}

uint32_t fmix32(uint32_t h)
{
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;

	return h;
}

void lmm_x86_128(const void *key, int len, uint32_t seed, void *out)
{
	const uint8_t *data = (const uint8_t *)key;
	const int nblocks = len / 16;

	uint32_t h1 = seed;
	uint32_t h2 = seed;
	uint32_t h3 = seed;
	uint32_t h4 = seed;

	const uint32_t c1 = 0x239b961b;
	const uint32_t c2 = 0xab0e9789;
	const uint32_t c3 = 0x38b34ae5;
	const uint32_t c4 = 0xa1e38b93;

	//----------
	// body

	const uint32_t *blocks = (const uint32_t *)(data + nblocks * 16);

	for (int i = -nblocks; i; i++) {
		uint32_t k1 = getblock32(blocks, i * 4 + 0);
		uint32_t k2 = getblock32(blocks, i * 4 + 1);
		uint32_t k3 = getblock32(blocks, i * 4 + 2);
		uint32_t k4 = getblock32(blocks, i * 4 + 3);

		k1 *= c1;
		k1 = ROTL32(k1, 15);
		k1 *= c2;
		h1 ^= k1;

		h1 = ROTL32(h1, 19);
		h1 += h2;
		h1 = h1 * 5 + 0x561ccd1b;

		k2 *= c2;
		k2 = ROTL32(k2, 16);
		k2 *= c3;
		h2 ^= k2;

		h2 = ROTL32(h2, 17);
		h2 += h3;
		h2 = h2 * 5 + 0x0bcaa747;

		k3 *= c3;
		k3 = ROTL32(k3, 17);
		k3 *= c4;
		h3 ^= k3;

		h3 = ROTL32(h3, 15);
		h3 += h4;
		h3 = h3 * 5 + 0x96cd1c35;

		k4 *= c4;
		k4 = ROTL32(k4, 18);
		k4 *= c1;
		h4 ^= k4;

		h4 = ROTL32(h4, 13);
		h4 += h1;
		h4 = h4 * 5 + 0x32ac3b17;
	}

	//----------
	// tail

	const uint8_t *tail = (const uint8_t *)(data + nblocks * 16);

	uint32_t k1 = 0;
	uint32_t k2 = 0;
	uint32_t k3 = 0;
	uint32_t k4 = 0;

	switch (len & 15) {
		case 15: k4 ^= tail[14] << 16;
		case 14: k4 ^= tail[13] << 8;
		case 13:
			k4 ^= tail[12] << 0;
			k4 *= c4;
			k4 = ROTL32(k4, 18);
			k4 *= c1;
			h4 ^= k4;

		case 12: k3 ^= tail[11] << 24;
		case 11: k3 ^= tail[10] << 16;
		case 10: k3 ^= tail[9] << 8;
		case 9:
			k3 ^= tail[8] << 0;
			k3 *= c3;
			k3 = ROTL32(k3, 17);
			k3 *= c4;
			h3 ^= k3;

		case 8: k2 ^= tail[7] << 24;
		case 7: k2 ^= tail[6] << 16;
		case 6: k2 ^= tail[5] << 8;
		case 5:
			k2 ^= tail[4] << 0;
			k2 *= c2;
			k2 = ROTL32(k2, 16);
			k2 *= c3;
			h2 ^= k2;

		case 4: k1 ^= tail[3] << 24;
		case 3: k1 ^= tail[2] << 16;
		case 2: k1 ^= tail[1] << 8;
		case 1:
			k1 ^= tail[0] << 0;
			k1 *= c1;
			k1 = ROTL32(k1, 15);
			k1 *= c2;
			h1 ^= k1;
	};

	//----------
	// finalization

	h1 ^= len;
	h2 ^= len;
	h3 ^= len;
	h4 ^= len;

	h1 += h2;
	h1 += h3;
	h1 += h4;
	h2 += h1;
	h3 += h1;
	h4 += h1;

	h1 = fmix32(h1);
	h2 = fmix32(h2);
	h3 = fmix32(h3);
	h4 = fmix32(h4);

	h1 += h2;
	h1 += h3;
	h1 += h4;
	h2 += h1;
	h3 += h1;
	h4 += h1;

	((uint32_t *)out)[0] = h1;
	((uint32_t *)out)[1] = h2;
	((uint32_t *)out)[2] = h3;
	((uint32_t *)out)[3] = h4;
}
weak_alias(lmm_x86_128, MurmurHash3_x86_128);

#if __BYTE_ORDER == __ORDER_LITTLE_ENDIAN
#define READ_UINT64(ptr) (*((uint64_t *)(ptr)))
#elif __BYTE_ORDER == __BIG_ENDIAN
#if defined(__GNUC__) &&                                                       \
	(__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3))
#define READ_UINT64(ptr) (__builtin_bswap64(*((uint64_t *)(ptr))))
#else
#define READ_UINT64(ptr) (ptr[0] | ptr[1] << 8 | ptr[2] << 16 | ptr[3] << 24 | ptr[4] << 32 | ptr[5] << 40 | ptr[6] << 48 | ptr[7] << 56)
#define UNALIGNED_SAFE
#endif
#else
#define READ_UINT64(ptr) (ptr[0] | ptr[1] << 8 | ptr[2] << 16 | ptr[3] << 24 | ptr[4] << 32 | ptr[5] << 40 | ptr[6] << 48 | ptr[7] << 56)
#define UNALIGNED_SAFE
#endif

#define ROTL64(x, r) (((uint64_t)x << r) | ((uint64_t)x >> (64 - r)))

uint64_t getblock64(const unsigned char *addr, int offset)
{
	unsigned char data[sizeof(uint64_t)];
	memcpy(&data, addr + offset * sizeof(uint64_t), sizeof(uint64_t));
	return READ_UINT64(data);
}

#define BIG_CONSTANT(x) (x##LLU)

uint64_t fmix64(uint64_t k)
{
	k ^= k >> 33;
	k *= BIG_CONSTANT(0xff51afd7ed558ccd);
	k ^= k >> 33;
	k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
	k ^= k >> 33;

	return k;
}

void lmm_x64_128(const void *key, const int len, const uint32_t seed,
						 void *out)
{
	const uint8_t *data = (const uint8_t *)key;
	const int nblocks = len / 16;

	uint64_t h1 = seed;
	uint64_t h2 = seed;

	const uint64_t c1 = BIG_CONSTANT(0x87c37b91114253d5);
	const uint64_t c2 = BIG_CONSTANT(0x4cf5ad432745937f);

	//----------
	// body

	const uint64_t *blocks = (const uint64_t *)(data);

	for (int i = 0; i < nblocks; i++) {
		uint64_t k1 = getblock64(blocks, i * 2 + 0);
		uint64_t k2 = getblock64(blocks, i * 2 + 1);

		k1 *= c1;
		k1 = ROTL64(k1, 31);
		k1 *= c2;
		h1 ^= k1;

		h1 = ROTL64(h1, 27);
		h1 += h2;
		h1 = h1 * 5 + 0x52dce729;

		k2 *= c2;
		k2 = ROTL64(k2, 33);
		k2 *= c1;
		h2 ^= k2;

		h2 = ROTL64(h2, 31);
		h2 += h1;
		h2 = h2 * 5 + 0x38495ab5;
	}

	//----------
	// tail

	const uint8_t *tail = (const uint8_t *)(data + nblocks * 16);

	uint64_t k1 = 0;
	uint64_t k2 = 0;

	switch (len & 15) {
		case 15: k2 ^= ((uint64_t)tail[14]) << 48;
		case 14: k2 ^= ((uint64_t)tail[13]) << 40;
		case 13: k2 ^= ((uint64_t)tail[12]) << 32;
		case 12: k2 ^= ((uint64_t)tail[11]) << 24;
		case 11: k2 ^= ((uint64_t)tail[10]) << 16;
		case 10: k2 ^= ((uint64_t)tail[9]) << 8;
		case 9:
			k2 ^= ((uint64_t)tail[8]) << 0;
			k2 *= c2;
			k2 = ROTL64(k2, 33);
			k2 *= c1;
			h2 ^= k2;

		case 8: k1 ^= ((uint64_t)tail[7]) << 56;
		case 7: k1 ^= ((uint64_t)tail[6]) << 48;
		case 6: k1 ^= ((uint64_t)tail[5]) << 40;
		case 5: k1 ^= ((uint64_t)tail[4]) << 32;
		case 4: k1 ^= ((uint64_t)tail[3]) << 24;
		case 3: k1 ^= ((uint64_t)tail[2]) << 16;
		case 2: k1 ^= ((uint64_t)tail[1]) << 8;
		case 1:
			k1 ^= ((uint64_t)tail[0]) << 0;
			k1 *= c1;
			k1 = ROTL64(k1, 31);
			k1 *= c2;
			h1 ^= k1;
	};

	//----------
	// finalization

	h1 ^= len;
	h2 ^= len;

	h1 += h2;
	h2 += h1;

	h1 = fmix64(h1);
	h2 = fmix64(h2);

	h1 += h2;
	h2 += h1;

	((uint64_t *)out)[0] = h1;
	((uint64_t *)out)[1] = h2;
}
weak_alias(lmm_x64_128, MurmurHash3_x64_128);
