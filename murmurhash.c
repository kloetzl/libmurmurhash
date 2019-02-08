//-----------------------------------------------------------------------------
// libmurmurhash was written by Fabian Klötzl, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#include "murmurhash.h"
#include "PMurHash.h"
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void lmmh_x86_32(const void *addr, unsigned int len, MH_UINT32 seed,
				 uint32_t out[1])
{
	if (len < (unsigned int)INT_MAX) {
		MH_UINT32 hash = PMurHash32(seed, addr, len);
		memcpy(out, &hash, sizeof(hash));
	} else {
		uint32_t h1 = seed, carry = 0;
		PMurHash32_Process(&h1, &carry, addr, INT_MAX);
		PMurHash32_Process(&h1, &carry, addr + INT_MAX, len - INT_MAX);
		MH_UINT32 hash = PMurHash32_Result(h1, carry, len);
		memcpy(out, &hash, sizeof(hash));
	}
}

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define READ_UINT32(ptr) (*((uint32_t *)(ptr)))
#define READ_UINT64(ptr) (*((uint64_t *)(ptr)))

#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define READ_UINT32(ptr) (__builtin_bswap32(*((uint32_t *)(ptr))))
#define READ_UINT64(ptr) (__builtin_bswap64(*((uint64_t *)(ptr))))

#else
#error "weird byte order"
#endif

/* Please don't send me emails about how this is undefined behaviour. */
#define ROTL32(x, r) (((uint32_t)x << r) | ((uint32_t)x >> (32 - r)))
#define ROTL64(x, r) (((uint64_t)x << r) | ((uint64_t)x >> (64 - r)))

static uint32_t getblock32(const void *addr, int offset)
{
	unsigned char data[sizeof(uint32_t)];
	memcpy(&data, addr + offset * sizeof(uint32_t), sizeof(uint32_t));
	return READ_UINT32(data);
}

static uint32_t fmix32(uint32_t h)
{
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;

	return h;
}

void lmmh_x86_128(const void *addr, unsigned int len, uint32_t seed,
				  uint32_t out[4])
{
	const uint8_t *data = (const uint8_t *)addr;
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

	const uint8_t *ptr = data + nblocks * 16;
	for (int i = -nblocks; i; i++) {
		uint32_t k1 = getblock32(ptr, i * 4 + 0);
		uint32_t k2 = getblock32(ptr, i * 4 + 1);
		uint32_t k3 = getblock32(ptr, i * 4 + 2);
		uint32_t k4 = getblock32(ptr, i * 4 + 3);

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

static uint64_t getblock64(const unsigned char *addr, int offset)
{
	unsigned char data[sizeof(uint64_t)];
	memcpy(&data, addr + offset * sizeof(uint64_t), sizeof(uint64_t));
	return READ_UINT64(data);
}

#define BIG_CONSTANT(x) (x##LLU)

static uint64_t fmix64(uint64_t k)
{
	k ^= k >> 33;
	k *= BIG_CONSTANT(0xff51afd7ed558ccd);
	k ^= k >> 33;
	k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
	k ^= k >> 33;

	return k;
}

void lmmh_x64_128(const void *addr, unsigned int len, uint32_t seed,
				  uint64_t out[2])
{
	const uint8_t *data = (const uint8_t *)addr;
	const int nblocks = len / 16;

	uint64_t h1 = seed;
	uint64_t h2 = seed;

	const uint64_t c1 = BIG_CONSTANT(0x87c37b91114253d5);
	const uint64_t c2 = BIG_CONSTANT(0x4cf5ad432745937f);

	//----------
	// body

	for (int i = 0; i < nblocks; i++) {
		uint64_t k1 = getblock64(data, i * 2 + 0);
		uint64_t k2 = getblock64(data, i * 2 + 1);

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

void MurmurHash3_x86_32(const void *addr, int len, uint32_t seed, void *out)
{
	uint32_t buffer;
	lmmh_x86_32(addr, len, seed, &buffer);
	memcpy(out, &buffer, sizeof(buffer));
}

void MurmurHash3_x86_128(const void *addr, int len, uint32_t seed, void *out)
{
	uint32_t buffer[4];
	lmmh_x86_128(addr, len, seed, buffer);
	memcpy(out, buffer, sizeof(buffer));
}

void MurmurHash3_x64_128(const void *addr, int len, uint32_t seed, void *out)
{
	uint64_t buffer[2];
	lmmh_x64_128(addr, len, seed, buffer);
	memcpy(out, buffer, sizeof(buffer));
}
