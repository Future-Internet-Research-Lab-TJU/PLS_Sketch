#ifndef PCAPREADER_H
#define PCAPREADER_H

#include <cstdint>
#include <string>
#include <vector>

static uint16_t read_be16(const uint8_t* p) {
return (p[0] << 8) | p[1];
}

// IPv4 address
static std::string ipv4_to_str(uint32_t addr) {
uint8_t b1 = (addr >> 24) & 0xFF;
uint8_t b2 = (addr >> 16) & 0xFF;
uint8_t b3 = (addr >> 8) & 0xFF;
uint8_t b4 = addr & 0xFF;
return std::to_string(b1) + "." + std::to_string(b2) + "." + std::to_string(b3) + "." + std::to_string(b4);
}

#pragma pack(push, 1)
struct PcapHeader {
uint32_t magic;
uint16_t vmajor;
uint16_t vminor;
int32_t thiszone;
uint32_t sigfigs;
uint32_t snaplen;
uint32_t network;
};

struct PcapRecHeader {
uint32_t ts_sec;
uint32_t ts_usec;
uint32_t incl_len;
uint32_t orig_len;
};
#pragma pack(pop)

std::vector<std::pair<uint32_t, uint32_t>> read_pcap(const std::string& filename, size_t max_count);

#endif // PCAPREADER_H