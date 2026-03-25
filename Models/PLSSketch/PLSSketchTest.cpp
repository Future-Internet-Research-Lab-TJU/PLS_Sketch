#include "PLSSketchTest.h"

#include "PLSSketch.h"
#include "utils.h"

#include <cmath>
#include <print>

TestResult test_PLSSketch(const PLSSketchParam &params, const std::vector<Packet> &packets, const std::unordered_set<Key> &expected)
{
    PLSSketch sketch(PLSSketch::Param{
        .row_size = params.row_size,
        .colume_size = params.colume_size,
        .estimator_size = params.estimator_size,
        .cardinality_weight = params.cardinality_weight,
        .ar_weight = params.ar_weight,
        .score_alpha = params.score_alpha,
        .remove_threshold = params.remove_threshold});

    std::vector<Key> keys;

    auto insert_task = [&packets, &sketch, &keys, &params, time_window_size = params.time_window_size]()
    {
        for (size_t i = 0; i < packets.size(); i++)
        {
            const auto &packet = packets[i];
            const int time = i / time_window_size;
            sketch.insert(packet.src_addr, packet.dst_addr, time);
        }
    };

    auto time = Utils::time_it(insert_task);

    auto query_time = Utils::repeat(100, [&]()
                                       { sketch.query(params.threshold); });

    auto sketch_result = sketch.query(params.threshold);

    // Calculate metrics
    size_t true_positive = 0;
    size_t false_positive = 0;
    size_t false_negative = 0;

    for (const auto &key : sketch_result)
    {
        if (expected.find(key) != expected.end())
        {
            true_positive++;
        }
        else
        {
            false_positive++;
        }
    }

    for (const auto &key : expected)
    {
        auto it = std::find(sketch_result.begin(), sketch_result.end(), key);
        if (it == sketch_result.end())
        {
            false_negative++;
        }
    }

    TestResult result;
    result.memory = params.row_size * params.colume_size * (sizeof(Key) + params.estimator_size / 8 + 8 + 2 + 1);
    result.time = time;
    result.throughput = packets.size() / (time / 1000000000.0);
    result.query_time = query_time;

    result.precise = (true_positive + false_positive > 0) ? static_cast<double>(true_positive) / (true_positive + false_positive) : 0.0;
    result.recall = (true_positive + false_negative > 0) ? static_cast<double>(true_positive) / (true_positive + false_negative) : 0.0;
    result.f1_score = (result.precise + result.recall > 0) ? 2 * result.precise * result.recall / (result.precise + result.recall) : 0.0;

    return result;
}
