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

inline Coordinates operator+( const Coordinates &a, const Coordinates &b )
{
    return Coordinates
    {
        a.x + b.x,
        a.y + b.y
    };
}

enum class Rgba_component
{
    red,
    green,
    blue,
    alpha,
    count,
    rgb_count = 3
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
