#include "HyperLogLog.h"

#include "murmur.h"
#include "utils.h"

#include <cassert>
#include <print>

HyperLogLog::HyperLogLog(size_t counter_size, size_t counter_num)
    : counter_size(counter_size), counter_num(counter_num)
{
    assert(counter_size <= 8);
    const size_t total_size = (counter_size * counter_num + 15) / 8;
    counters.resize(total_size, 0);
    this->alpha = Utils::correct_factor(counter_num);
}

void HyperLogLog::insert(const Element &element)
{
    uint32_t hash_values[4] = {0};
    MurmurHash3_x64_128(&element, sizeof(element), 0, hash_values);
    const size_t bucket_index = hash_values[0] % counter_num;
    const size_t leading_zeros = std::countr_zero(hash_values[1]);

    const size_t pos = bucket_index * counter_size; 
    size_t byte_index = pos / 8;
    size_t bit_index = pos % 8;
    uint16_t &counter = *reinterpret_cast<uint16_t *>(counters.data() + byte_index);

    const uint8_t mask = (1 << counter_size) - 1;
    const uint8_t count = (counter >> bit_index) & mask;
    const uint8_t max_count = std::pow(2, this->counter_size) - 1;
    const uint16_t new_count = std::min(max_count, std::max(count, static_cast<uint8_t>(leading_zeros + 1)));

    counter &= ~(static_cast<uint16_t>(mask) << bit_index);
    counter |= (new_count << bit_index);
}

void HyperLogLog::insert_hashed(uint64_t hash_value)
{
    const size_t bucket_index = hash_value % counter_num;
    const size_t leading_zeros = std::countr_zero(hash_value >> 32);

    const size_t pos = bucket_index * counter_size; 
    size_t byte_index = pos / 8;
    size_t bit_index = pos % 8;
    uint16_t &counter = *reinterpret_cast<uint16_t *>(counters.data() + byte_index);

    const uint8_t mask = (1 << counter_size) - 1;
    const uint8_t count = (counter >> bit_index) & mask;
    const uint16_t new_count = std::max(count, static_cast<uint8_t>(leading_zeros + 1));

    counter &= ~(static_cast<uint16_t>(mask) << bit_index);
    counter |= (new_count << bit_index);
}

double HyperLogLog::estimate() const
{
    double estimate_value = 0.0;
    size_t empty_bucket_num = 0;
    for (size_t i = 0; i < counter_num; ++i)
    {
        const size_t pos = i * counter_size;
        size_t byte_index = pos / 8;
        size_t bit_index = pos % 8;
        const uint16_t counter = *reinterpret_cast<const uint16_t *>(counters.data() + byte_index);
        const uint8_t count = (counter >> bit_index) & ((1 << counter_size) - 1);
        if (count == 0)
        {
            ++empty_bucket_num;
        }
        estimate_value += 1.0 / (1 << count);
    }
    estimate_value = this->counter_num * this->counter_num * this->alpha / estimate_value;
    estimate_value = Utils::correct_estimation(estimate_value, this->counter_num, empty_bucket_num);
    return estimate_value;
}

void HyperLogLog::reset()
{
    std::fill(counters.begin(), counters.end(), 0);
}
