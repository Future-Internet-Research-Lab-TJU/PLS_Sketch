#include "Data.h"

#include "PcapReader.h"
#include "reader.h"

#include <cmath>
#include <fstream>
#include <print>
#include <unordered_map>
#include <unordered_set>

std::vector<Packet> read_packets(const std::string &file_path, size_t count)
{
    std::vector<Packet> packets;
    packets.reserve(count);

    Reader reader(file_path);

    while (packets.size() < count)
    {
        auto packet = reader.read();
        if (packet.version != 4)
            continue;
        const auto srcAddr = toBinary(packet.srcAddr);
        const auto dstAddr = toBinary(packet.dstAddr);
        packets.emplace_back(srcAddr, dstAddr);
    }

    return packets;
}

std::vector<Packet> read_pcap_packets(const std::string &file_path, size_t count)
{
    std::vector<Packet> packets;
    auto pairs = read_pcap(file_path, count);
    for (const auto &pair : pairs)
    {
        packets.emplace_back(pair.first, pair.second);
    }
    return packets;
}

std::vector<Packet> read_web_packets(const std::string &file_path, size_t count)
{
    std::vector<Packet> packets;
    std::ifstream file(file_path, std::ios::binary);
    uint8_t buffer[13];
    while (packets.size() < count && !file.eof())
    {
        file.read((char*)buffer, 13);
        Packet packet;
        memcpy(&packet.src_addr, buffer, 4);
        memcpy(&packet.dst_addr, buffer + 4, 4);
        packets.emplace_back(packet);
    }
    return packets;
}

struct Value
{
    double arrival_rate;
    size_t max_gap_count;
    size_t sample_num;
    size_t max_element;
};

Value evaluate(const std::vector<uint32_t> &element)
{
    Value result;

    auto max_element = *std::max_element(element.begin(), element.end());
    result.max_element = max_element;

    auto start_it = std::find_if(element.begin(), element.end(),
                                 [](uint32_t count)
                                 { return count > 0; });
    auto end_it = std::find_if(element.rbegin(), element.rend(),
                               [](uint32_t count)
                               { return count > 0; })
                      .base();

    auto sample_num = std::distance(start_it, end_it);
    result.sample_num = sample_num;

    double arrival_rate = 0;
    for (auto it = start_it; it != end_it; ++it)
    {
        if (*it > 0)
            arrival_rate += 1;
    }
    arrival_rate /= (end_it - start_it);
    result.arrival_rate = arrival_rate;

    size_t max_gap_count = 0;
    size_t current_gap_count = 0;
    for (auto it = start_it; it != end_it; ++it)
    {
        if (*it == 0)
        {
            current_gap_count++;
            if (current_gap_count > max_gap_count)
                max_gap_count = current_gap_count;
        }
        else
        {
            current_gap_count = 0;
        }
    }
    result.max_gap_count = max_gap_count;

    return result;
}

std::unordered_map<Key, std::vector<bool>> find_persistent_and_unpopular(const std::vector<Packet> &packets,
                                                                         size_t time_window_size,
                                                                         double arrival_rate_threshold,
                                                                         size_t max_gap,
                                                                         size_t popularity_threshold,
                                                                         size_t min_sample_num)
{
    std::unordered_map<Key, std::vector<uint32_t>> features;
    std::unordered_map<Key, std::unordered_set<Element>> popularity;

    const size_t time_window_count = std::ceil(packets.size() * 1.0 / time_window_size);

    for (size_t i = 0; i < packets.size(); ++i)
    {
        const auto &packet = packets[i];
        auto popularity_it = popularity.find(packet.src_addr);
        if (popularity_it == popularity.end())
        {
            popularity_it = popularity.emplace(packet.src_addr, std::unordered_set<Element>()).first;
        }
        popularity_it->second.insert(packet.dst_addr);

        if ((i + 1) % time_window_size == 0)
        {
            const auto current_time_window = i / time_window_size;
            for (const auto &[key, elements] : popularity)
            {
                auto feature_it = features.find(key);
                if (feature_it == features.end())
                {
                    feature_it = features.emplace(key, std::vector<uint32_t>(time_window_count, 0)).first;
                }
                feature_it->second[current_time_window] = elements.size();
            }
            popularity.clear();
        }
    }
    if (!popularity.empty())
    {
        for (const auto &[key, elements] : popularity)
        {
            auto feature_it = features.find(key);
            if (feature_it == features.end())
            {
                feature_it = features.emplace(key, std::vector<uint32_t>(time_window_count, 0)).first;
            }
            feature_it->second.back() = elements.size();
        }
    }

    std::unordered_map<Key, std::vector<bool>> persistent_and_unpopular;
    for (const auto &[key, element_counts] : features)
    {
        Value value = evaluate(element_counts);
        if (value.arrival_rate < arrival_rate_threshold ||
            value.max_gap_count > max_gap ||
            value.sample_num < min_sample_num)
        {
            continue;
        }

        std::vector<bool> result(time_window_count, false);
        for (size_t i = 0; i < time_window_count; ++i)
        {
            if (element_counts[i] != 0)
                result[i] = true;
        }
        persistent_and_unpopular.emplace(key, result);
    }

    return persistent_and_unpopular;
}

std::unordered_set<Key> extract_keys(const std::unordered_map<Key, std::vector<bool>>& map)
{
    std::unordered_set<Key> keys;
    for (const auto &[key, value] : map)
    {
        keys.insert(key);
    }
    return keys;
}

void TestResult::print(const std::string &name) const
{
    std::print("Test result for [ {} ]:\n\
                Memory consumption:     {:.2f} KB\n\
                Time taken:             {:.2f} ms\n\
                Insert Throughput:      {:.2f} Mpps\n\
                Query Time:             {:.2f} us\n\
                Precision:              {:.2f}\n\
                Recall:                 {:.2f}\n\
                F1 Score:               {:.2f}\n",
               name,
               memory / 1024.0,
               time / 1000000.0,
               throughput / 1000000.0,
               query_time / 1000.0,
               precise * 100,
               recall * 100,
               f1_score * 100);
}
