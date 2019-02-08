//-----------------------------------------------------------------------------
// libmurmurhash was written by Fabian Klötzl, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>

/** @brief Compute a non-cryptographic hash value.
 *
 * Consume the seed and a number of bytes starting from the pointer. Chews on
 * the data and produces a hash value. Do not use for cryptography.
 *
 * @param addr - Pointer to data.
 * @param len - Length of data.
 * @param seed - Influence the resulting hash value.
 * @param out - Output parameter.
 */
extern void lmmh_x86_32(const void *addr, unsigned int len, uint32_t seed,
						uint32_t out[1]);
extern void lmmh_x86_128(const void *addr, unsigned int len, uint32_t seed,
						 uint32_t out[4]);
extern void lmmh_x64_128(const void *addr, unsigned int len, uint32_t seed,
						 uint64_t out[2]);

// The original function names still work but are deprecated in favor of the
// lmmh_* interface
extern __attribute__((deprecated)) void
MurmurHash3_x86_32(const void *data, int len, uint32_t seed, void *out);
extern __attribute__((deprecated)) void
MurmurHash3_x86_128(const void *data, int len, uint32_t seed, void *out);
extern __attribute__((deprecated)) void
MurmurHash3_x64_128(const void *data, int len, uint32_t seed, void *out);

#ifdef __cplusplus
}
#endif
