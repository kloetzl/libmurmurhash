//-----------------------------------------------------------------------------
// libmurmurhash was written by Fabian Klötzl, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#pragma once

#ifdef __cplusplus__
extern "C" {
#endif
#include <stdint.h>

extern void lmmh_x86_32(const void *key, int len, uint32_t seed, void *out);
extern void lmmh_x86_128(const void *key, int len, uint32_t seed, void *out);
extern void lmmh_x64_128(const void *key, const int len, const uint32_t seed,
						 void *out);

// old interface:
extern void MurmurHash3_x86_32(const void *key, int len, uint32_t seed,
							   void *out)
	__attribute__((weak));
extern void MurmurHash3_x86_128(const void *key, int len, uint32_t seed,
								void *out)
	__attribute__((weak));
extern void MurmurHash3_x64_128(const void *key, int len, uint32_t seed,
								void *out)
	__attribute__((weak));

#ifdef __cplusplus__
}
#endif
