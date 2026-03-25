
#include <cmath>
#include <future>
#include <print>

#include "HyperLogLog.h"
#include "LinearCounting.h"
#include "PLSSketchTest.h"

#include "reader.h"

void test();

int main(int argc, char **argv)
{
    test();
}

void test()
{
    std::print("Testing ......\n");
    // read packets from dataset
    constexpr size_t count = 100'0000;
    const std::string dataset_path = "../Dataset/1.pcap";
    const std::vector<Packet> packets = read_packets(dataset_path, count);

    std::unordered_set<Key> flows;
    for (const auto &packet : packets)
    {
        flows.insert(packet.src_addr);
    }
    std::print("flows count: {}.\n", flows.size());

    // find expected persistent and unpopular flows
    constexpr size_t time_window_size = count / 100;
    const size_t time_window_num = std::ceil(packets.size() * 1.0 / time_window_size);
    constexpr double arrival_rate_threshold = 0.7;
    constexpr size_t max_gap = 100;
    constexpr size_t popularity_threshold = 5;
    constexpr size_t min_sample_num = 50;
    const auto expected = find_persistent_and_unpopular(packets,
                                                        time_window_size,
                                                        arrival_rate_threshold,
                                                        max_gap,
                                                        popularity_threshold,
                                                        min_sample_num);
    const auto expected_keys = extract_keys(expected);
    std::print("Found {} persistent and unpopular flows.\n", expected.size());

    auto test_PLSSketch_func = [&packets, expected = &expected_keys, time_window_size]()
    {
        constexpr double c_w = 0.5;
        constexpr double ar_w = 9.5;
        constexpr double r_t = 4.0;
        constexpr double t = 7.0;
        constexpr size_t mem = 200 * 1024;
        auto params = PLSSketchParam{
            .row_size = 3,
            .colume_size = mem / 3 / (23),
            .estimator_size = 64,
            .cardinality_weight = c_w,
            .ar_weight = ar_w,
            .score_alpha = 0.4,
            .remove_threshold = r_t,
            .threshold = t,
            .time_window_size = time_window_size};
        return test_PLSSketch(params, packets, *expected);
    };
    auto result_PLSSketch = std::async(std::launch::async, test_PLSSketch_func);
    result_PLSSketch.get().print("PLSSketch");
}