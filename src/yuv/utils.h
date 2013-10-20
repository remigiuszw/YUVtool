#ifndef UTILS_H
#define UTILS_H

#include <cstdint>

typedef std::uint8_t Byte;
const int bits_in_byte = 8;

struct Coordinates
{
    int x;
    int y;
};

enum class Rgba_component
{
    red,
    green,
    blue,
    alpha,
    count
};

template< typename TNumber >
TNumber round_down( TNumber value, TNumber divisor )
{
    return value - value%divisor;
}

template< typename TNumber >
TNumber round_up( TNumber value, TNumber divisor )
{
    return round_down( value + divisor - static_cast<TNumber>(1), divisor );
}

#endif // UTILS_H
