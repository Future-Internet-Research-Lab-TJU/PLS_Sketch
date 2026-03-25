#ifndef PLSSKETCH_H
#define PLSSKETCH_H

#include "Data.h"
#include "HyperLogLog.h"
#include "LinearCounting.h"

#include <vector>

class PLSSketch
{
public:
    struct Param
    {
        size_t row_size;
        size_t colume_size;
        size_t estimator_size;
        double cardinality_weight;
        double ar_weight;
        double score_alpha;
        double remove_threshold;
        double threshold;
    };
    PLSSketch(const Param &param);

public:
    void insert(const Key &key, const Element &element, size_t time);
    std::vector<Key> query(double threshold) const;

private:
    struct Bucket;
    std::vector<std::vector<Bucket>> buckets;
    Param param;
};

struct PLSSketch::Bucket
{
    Key key;
    LinearCounting estimator {64};
    uint64_t time_mask;
    uint16_t last_time;
    double score;
};

#endif /* PLSSKETCH_H */