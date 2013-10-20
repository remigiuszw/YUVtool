#include "picture_buffer.h"

Picture_buffer::Picture_buffer()
{
}
//------------------------------------------------------------------------------
Picture_buffer::~Picture_buffer()
{
}
//------------------------------------------------------------------------------
void Picture_buffer::allocate( const Coordinates &resolution,
    const Pixel_format &pixel_format )
{
    m_resolution = resolution;
    m_pixel_format = pixel_format;
    const int frame_size =
        m_resolution.x * m_resolution.y *
        get_bits_per_macropixel( m_pixel_format ) /
        m_pixel_format.m_macropixel_coding.m_coded_pixels.size() /
        bits_in_byte;
    m_data.resize( frame_size );
}
//------------------------------------------------------------------------------
const Pixel_format &Picture_buffer::get_pixel_format() const
{
    return m_pixel_format;
}
//------------------------------------------------------------------------------
Coordinates Picture_buffer::get_resolution() const
{
    return m_resolution;
}
//------------------------------------------------------------------------------
std::vector< Byte > &Picture_buffer::get_data()
{
    return m_data;
}
//------------------------------------------------------------------------------
const std::vector< Byte > &Picture_buffer::get_data() const
{
    return m_data;
}
