#ifndef HYPERLOGLOG_H
#define HYPERLOGLOG_H

#include "Data.h"

#include <cstdint>
#include <vector>

class HyperLogLog
{
public:
    HyperLogLog(size_t counter_size, size_t counter_num);

public:
    void insert(const Element &element);
    void insert_hashed(uint64_t hash_value);
    double estimate() const;
    void reset();

private:
    std::vector<uint8_t> counters;
    size_t counter_size;
    size_t counter_num;
    double alpha;
};

#endif // HYPERLOGLOG_H