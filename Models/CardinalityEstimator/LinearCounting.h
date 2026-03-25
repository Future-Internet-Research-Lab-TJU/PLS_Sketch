#ifndef LINEARCOUNTING_H
#define LINEARCOUNTING_H

#include <cstdint>
#include <vector>

class LinearCounting
{
public:
    explicit LinearCounting(size_t bit_vector_size);
    void insert(const uint32_t &element);
    double estimate() const;
    void reset();

private:
    size_t bit_vector_size;
    std::vector<uint8_t> bit_vector;
};

#endif // LINEARCOUNTING_H