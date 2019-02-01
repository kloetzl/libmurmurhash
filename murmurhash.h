//-----------------------------------------------------------------------------
// libmurmurhash was written by Fabian Klötzl, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#pragma once

#ifdef __cplusplus__
extern "C" {
#endif
#include <stdint.h>

// old interface:
extern void MurmurHash3_x86_32(const void *key, int len, uint32_t seed,
							   void *out);
extern void MurmurHash3_x86_128(const void *key, int len, uint32_t seed,
								void *out);
extern void MurmurHash3_x64_128(const void *key, int len, uint32_t seed,
								void *out);

#ifdef __cplusplus__
}
#endif
