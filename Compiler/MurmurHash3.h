#ifndef _MURMURHASH3_H_
#define _MURMURHASH3_H_

//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.
//-----------------------------------------------------------------------------

#include <cstdint>

//-----------------------------------------------------------------------------

void MurmurHash3_x86_32(const void *key, int len, uint32_t seed, void *out);
uint32_t MurmurHash3_x86_32(const void *data, const uint32_t size, const uint32_t seed);

void MurmurHash3_x86_128(const void *key, int len, uint32_t seed, void *out);
void MurmurHash3_x64_128(const void *key, int len, uint32_t seed, void *out);

//-----------------------------------------------------------------------------

#endif // _MURMURHASH3_H_
