#include "PLSSketch.h"

#include "murmur.h"

#include <cassert>
#include <cmath>
#include <fstream>

PLSSketch::PLSSketch(const Param &param)
    : buckets(param.row_size, std::vector<Bucket>(param.colume_size, Bucket{.time_mask = 0, .last_time = 0, .score = 0})),
      param(param)
{
    assert(param.row_size <= 4);
}

void PLSSketch::insert(const Key &key, const Element &element, size_t time)
{
    uint32_t key_hash_values[4] = {0};
    MurmurHash3_x64_128(&key, sizeof(key), 0, key_hash_values);
    for (size_t row = 0; row < buckets.size(); ++row)
    {
        const size_t col = key_hash_values[row] % buckets[row].size();
        Bucket &bucket = buckets[row][col];
        if (bucket.key == key)
        {
            if (time != bucket.last_time)
            {
                const auto cardinality = bucket.estimator.estimate();
                const size_t active_time = std::popcount(bucket.time_mask);
                double score = this->param.cardinality_weight * (1 / cardinality) +
                               this->param.ar_weight * active_time / 64;

                bucket.score = this->param.score_alpha * bucket.score + (1 - this->param.score_alpha) * score;

                bucket.estimator.reset();
                bucket.time_mask = bucket.time_mask << (time - bucket.last_time);
            }
            bucket.estimator.insert(element);
            bucket.time_mask |= 1;
            bucket.last_time = time;
            return;
        }
    }
    for (size_t row = 0; row < buckets.size(); ++row)
    {
        const size_t col = key_hash_values[row] % buckets[row].size();
        Bucket &bucket = buckets[row][col];
        if (bucket.key == 0)
        {
            bucket.key = key;
            bucket.estimator.insert(element);
            bucket.time_mask = 1;
            bucket.last_time = time;
            const auto cardinality = 1;
            const auto active_time = 1;
            const double score = this->param.cardinality_weight * (1 / cardinality) +
                                 (this->param.ar_weight) * active_time / 64;
            bucket.score = score;
            return;
        }
    }

    for (size_t row = 0; row < buckets.size(); ++row)
    {
        const size_t col = key_hash_values[row] % buckets[row].size();
        Bucket &bucket = buckets[row][col];
        double replace_score = bucket.score * this->param.remove_threshold;
        double delay_time = time - bucket.last_time;
        if (delay_time > replace_score)
        {
            bucket.key = key;
            bucket.estimator.insert(element);
            bucket.time_mask = 1;
            bucket.last_time = time;
            const auto cardinality = 1;
            const size_t active_time = 1;
            const auto score = this->param.cardinality_weight * (1 / cardinality) +
                               (this->param.ar_weight) * active_time / 64;
            bucket.score = score;
            return;
        }
    }
}

std::vector<Key> PLSSketch::query(double threshold) const
{
    std::vector<Key> result;
    double th = threshold;
    for (const auto &array : this->buckets)
    {
        for (const auto &bucket : array)
        {
            if (bucket.score > th)
            {
                result.emplace_back(bucket.key);
            }
        }
    }
    return result;
}
