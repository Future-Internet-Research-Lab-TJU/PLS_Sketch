#ifndef UTILS_H
#define UTILS_H

#include "murmur.h"

#include <chrono>
#include <cmath>

namespace Utils
{
    template <typename T, typename... Args>
    auto time_it(T func, Args &&...args)
    {
        auto start = std::chrono::high_resolution_clock::now();
        func(std::forward<Args>(args)...);
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    }

    template <typename T, typename... Args>
    auto repeat(size_t repeat_num, T func, Args &&...args)
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < repeat_num; i++)
            func(std::forward<Args>(args)...);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        return duration / repeat_num;
    }

    inline auto &hash = MurmurHash3_x64_128;
    // inline auto &hash32 = MurmurHash3_x86_32;
    uint32_t hash32(const void* data, size_t len, uint32_t seed = 0);
    uint64_t hash64(const void* data, size_t len, uint32_t seed = 0);

    constexpr double correct_factor(size_t bucket_num)
    {
        double factor = 0.0;
        if (bucket_num < 32)
            factor = 0.673;
        else if (bucket_num < 64)
            factor = 0.697;
        else if (bucket_num < 128)
            factor = 0.709;
        else
            factor = 0.7213 / (1.0 + 1.079 / (bucket_num * 1.0));
        return factor;
    }

    constexpr double correct_estimation(double estimate_value, size_t bucket_num, size_t empty_bucket_num)
    {
        if (estimate_value <= 2.5 * bucket_num)
        {
            if (empty_bucket_num != 0)
                estimate_value = bucket_num * std::log(static_cast<double>(bucket_num) / static_cast<double>(empty_bucket_num));
        }
        else if (estimate_value <= (1ull << 31) / 15.0)
        {
            estimate_value = estimate_value;
        }
        else
        {
            estimate_value = -std::pow(2, 32) * std::log(1 - estimate_value / std::pow(2, 32));
        }
        return estimate_value;
    }

} // namespace utils

#endif // UTILS_H