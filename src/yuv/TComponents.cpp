#include "TComponents.h"
#include <stdexcept>

int get_bits_per_macropixel( const Pixel_format &pixel_format )
{
    int result = 0;
    for( auto &plane : pixel_format.m_planes )
        for( auto &entry : plane.m_entries )
            result += entry.m_bit_width;
    return result;
}
//------------------------------------------------------------------------------
Coordinates get_macropixel_size( const Macropixel_coding &macropixel_coding )
{
    const int macropixel_width =
        macropixel_coding.coded_pixels_per_row_in_macropixel;
    const int macropixel_height =
        macropixel_coding.m_coded_pixels.size() /
            macropixel_coding.coded_pixels_per_row_in_macropixel;
    const int coded_pixels_count = macropixel_coding.m_coded_pixels.size();
    if( macropixel_width*macropixel_height !=
        coded_pixels_count )
    {
        throw std::runtime_error( "invalid macropixel coding" );
    }
    return Coordinates
    {
        macropixel_width,
        macropixel_height
    };
}
