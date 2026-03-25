#include "utils.h"

#include <cmath>

uint32_t Utils::hash32(const void *data, size_t len, uint32_t seed)
{
    uint32_t hash_value;
    MurmurHash3_x86_32(data, len, seed, &hash_value);
    return hash_value;
}

uint64_t Utils::hash64(const void *data, size_t len, uint32_t seed)
{
    uint64_t hash_value[2];
    MurmurHash3_x64_128(data, len, seed, &hash_value);
    return hash_value[0];
}
