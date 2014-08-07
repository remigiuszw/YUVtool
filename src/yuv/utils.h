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
    return Coordinates{
            a.x + b.x,
            a.y + b.y};
}

inline Coordinates operator-( const Coordinates &a, const Coordinates &b )
{
    return Coordinates{
            a.x - b.x,
            a.y - b.y};
}

inline Coordinates operator/( const Coordinates &a, const Coordinates &b )
{
    return Coordinates{
            a.x / b.x,
            a.y / b.y};
}

inline Coordinates operator*( const Coordinates &a, const Coordinates &b )
{
    return Coordinates{
            a.x * b.x,
            a.y * b.y};
}

inline bool operator==( const Coordinates &a, const Coordinates &b )
{
    return
            a.x == b.x
            && a.y == b.y;
}

inline bool operator!=( const Coordinates &a, const Coordinates &b )
{
    return !(a == b);
}

enum Rgba_component
{
    Rgba_component_red,
    Rgba_component_green,
    Rgba_component_blue,
    Rgba_component_alpha,
    Rgba_component_count,
    Rgba_component_rgb_count = 3
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
