#include "TComponents.h"

int get_bits_per_macropixel( const Pixel_format &pixel_format )
{
    int result = 0;
    for( auto &plane : pixel_format.m_planes )
        for( auto &entry : plane.m_entries )
            result += entry.m_bit_width;
    return result;
}
