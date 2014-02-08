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
//------------------------------------------------------------------------------
void Picture_buffer::fill_tile_rgb( Coordinates tile_start,
    Coordinates tile_end, Byte *data ) const
{
    const Coordinates macropixel_size = get_macropixel_size(
        get_pixel_format().m_macropixel_coding );
    //const int bytes_per_pixel_component = 1;
    Coordinates macropixel_coordinates;

    for( macropixel_coordinates.y=tile_start.y;
         macropixel_coordinates.y<tile_end.y;
         macropixel_coordinates.y+=macropixel_size.y )
    {
        for( macropixel_coordinates.x=tile_start.x;
             macropixel_coordinates.x<tile_end.x;
             macropixel_coordinates.x+=macropixel_size.x )
        {
            draw_macropixel( macropixel_coordinates
                /*TODO: address for output */ );
        }
    }
}
//------------------------------------------------------------------------------
void Picture_buffer::draw_macropixel( Coordinates coordinates ) const
{
    const Coordinates size = get_macropixel_size(
        get_pixel_format().m_macropixel_coding );
    Coordinates pixel_offset;
    for( pixel_offset.y=0;
         pixel_offset.y<size.y;
         pixel_offset.y++ )
    {
        for( pixel_offset.x=0;
             pixel_offset.x<size.x;
             pixel_offset.x++ )
        {
            double rgb_values[static_cast<int>(Rgba_component::rgb_count)] =
                { 0 };
            const Coded_pixel &coded_pixel =
                get_pixel_format().m_macropixel_coding.m_coded_pixels[
                    pixel_offset.y*size.x+pixel_offset.x];
            draw_pixel( coordinates+pixel_offset, coded_pixel, rgb_values );
        }
    }
}
//------------------------------------------------------------------------------
void Picture_buffer::draw_pixel( Coordinates coordinates,
    const Coded_pixel &coded_pixel,
    double (&data)[static_cast<int>(Rgba_component::rgb_count)] ) const
{
    for( auto &component_coding : coded_pixel.m_component_codings )
    {
        for( Rgba_component rgb_component = Rgba_component::red;
             rgb_component != Rgba_component::rgb_count;
             rgb_component = static_cast<Rgba_component>(
                 static_cast<int>(rgb_component)+1 ) )
        {
            //int source_value =
                m_data[m_plane_offsets[component_coding.m_plane_index]];
            //rgb_values[component] +=
        }
    }
}

