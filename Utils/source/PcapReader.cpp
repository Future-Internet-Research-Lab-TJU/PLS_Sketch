#include "PcapReader.h"

#include <iostream>
#include <fstream>

std::vector<std::pair<uint32_t, uint32_t>> read_pcap(const std::string& filename, size_t max_count)
{
    std::ifstream f(filename, std::ios::binary);
    if (!f.is_open())
    {
        std::cerr << "cannot open file" << std::endl;
        return {};
    }

    PcapHeader g;
    f.read(reinterpret_cast<char *>(&g), sizeof(g));
    if (!f)
    {
        std::cerr << "bad global header" << std::endl;
        return {};
    }

    bool swap = false;
    if (g.magic == 0xd4c3b2a1)
        swap = true;
    if (g.magic != 0xa1b2c3d4 && g.magic != 0xd4c3b2a1)
    {
        std::cerr << "unsupported pcap" << std::endl;
        return {};
    }

    std::vector<std::pair<uint32_t, uint32_t>> packets;
    while (packets.size() < max_count && !f.eof())
    {
        PcapRecHeader h;
        f.read(reinterpret_cast<char *>(&h), sizeof(h));
        if (!f)
            break;

        uint32_t len = h.incl_len;
        std::vector<uint8_t> buf(len);
        f.read(reinterpret_cast<char *>(buf.data()), len);
        if (!f)
            break;

        if (g.network != 1)
            continue;
        if (len < 14)
            continue;

        uint16_t ethType = read_be16(buf.data() + 12);
        const uint8_t *p = buf.data() + 14;
        size_t remain = len - 14;

        if (ethType == 0x0800)
        {
            if (remain < 20)
                continue;
            uint8_t vihl = p[0];
            uint8_t version = vihl >> 4;
            uint8_t ihl = (vihl & 0x0F) * 4;
            if (version != 4 || remain < ihl)
                continue;

            uint32_t src = (p[12] << 24) | (p[13] << 16) | (p[14] << 8) | p[15];
            uint32_t dst = (p[16] << 24) | (p[17] << 16) | (p[18] << 8) | p[19];
            uint8_t proto = p[9];

            // std::cout << "IPv4: " << ipv4_to_str(src) << " -> " << ipv4_to_str(dst) << " proto=" << unsigned(proto) << std::endl;
            packets.emplace_back(src, dst);
        }
        else if (ethType == 0x86DD)
        {
            if (remain < 40)
                continue;
            uint8_t version = p[0] >> 4;
            if (version != 6)
                continue;

            char src[40];
            char dst[40];
            std::snprintf(src, sizeof(src), "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
                          p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15],
                          p[16], p[17], p[18], p[19], p[20], p[21], p[22], p[23]);
            std::snprintf(dst, sizeof(dst), "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
                          p[24], p[25], p[26], p[27], p[28], p[29], p[30], p[31],
                          p[32], p[33], p[34], p[35], p[36], p[37], p[38], p[39]);

            uint8_t next = p[6];
            // std::cout << "IPv6: " << src << " -> " << dst << " next=" << unsigned(next) << std::endl;
        }
    }
    return packets;
}