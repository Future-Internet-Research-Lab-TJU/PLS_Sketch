#ifndef DATA_H
#define DATA_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using Key = uint32_t;
using Element = uint32_t;

struct Packet
{
    Key src_addr;
    uint32_t dst_addr;
};

struct TestResult
{
    size_t memory;
    size_t time;
    size_t throughput;
    double query_time;
    double precise;
    double recall;
    double f1_score;

    void print(const std::string &name) const;
};

std::vector<Packet> read_packets(const std::string &file_path, size_t count);

std::vector<Packet> read_pcap_packets(const std::string &file_path, size_t count);

std::vector<Packet> read_web_packets(const std::string &file_path, size_t count);

std::unordered_map<Key, std::vector<bool>> find_persistent_and_unpopular(const std::vector<Packet> &packets,
                                                      size_t time_window_size,
                                                      double arrival_rate,
                                                      size_t max_gap,
                                                      size_t popularity_threshold,
                                                      size_t min_sample_num);

std::unordered_set<Key> extract_keys(const std::unordered_map<Key, std::vector<bool>>& map);

#endif // DATA_H