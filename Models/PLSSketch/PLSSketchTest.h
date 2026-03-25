#ifndef PLSSKETCHTEST_H
#define PLSSKETCHTEST_H

#include "Data.h"

struct PLSSketchParam
{
    size_t row_size;
    size_t colume_size;
    size_t estimator_size;
    double cardinality_weight;
    double ar_weight;
    double score_alpha;
    double remove_threshold;
    double threshold;
    size_t time_window_size;
};

TestResult test_PLSSketch(const PLSSketchParam &params, const std::vector<Packet> &packets, const std::unordered_set<Key> &expected);

#endif // PLSSKETCHTEST_H