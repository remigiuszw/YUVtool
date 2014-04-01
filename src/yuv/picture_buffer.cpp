#include <algorithm>

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
    // data is assumed to be in form:
    // format: GL_RGB
    // type: GL_BYTE
    // as given to glTexImage2D

    const int output_bytes_per_pixel = 3;

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
            const int tile_width = tile_end.x - tile_start.x;
            const int pixel_number_in_tile =
                tile_width * macropixel_coordinates.y +
                macropixel_coordinates.x;
            draw_macropixel( macropixel_coordinates,
                data + output_bytes_per_pixel * pixel_number_in_tile );
        }
    }
}
//------------------------------------------------------------------------------
void Picture_buffer::draw_macropixel( Coordinates coordinates, Byte *data )
    const
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
            double rgb_values[Rgba_component_rgb_count] = { 0 };
            const Coded_pixel &coded_pixel =
                get_pixel_format().m_macropixel_coding.m_coded_pixels[
                    pixel_offset.y*size.x+pixel_offset.x];
            draw_pixel( coordinates+pixel_offset, coded_pixel, rgb_values );
        }
    }
}
//------------------------------------------------------------------------------
namespace
{
int get_bits( const Byte *data, int start_bit_index, int end_bit_index )
{
    // TODO: optimize!!!
    int result = 0;
    int mask = 1;
    for( int i=start_bit_index; i<end_bit_index; i++ )
    {
        const int byte_index = i/bits_in_byte;
        const int bit_in_byte = i%bits_in_byte;
        const int bit = ( data[byte_index] >> bit_in_byte ) & 1;
        result |= bit ? mask : 0;
        mask <<= 1;
    }
    return result;
}
}
//------------------------------------------------------------------------------
void Picture_buffer::draw_pixel( Coordinates coordinates,
    const Coded_pixel &coded_pixel,
    double (&result)[Rgba_component_rgb_count] ) const
{
    const Macropixel_coding &macropixel_coding =
        m_pixel_format.m_macropixel_coding;
    Coordinates macropixel_dimensions{
        macropixel_coding.coded_pixels_per_row_in_macropixel,
        static_cast<int>( macropixel_coding.m_coded_pixels.size() ) /
        macropixel_coding.coded_pixels_per_row_in_macropixel };
    Coordinates coordinates_in_macropixel_units{
        coordinates.x / macropixel_dimensions.x,
        coordinates.y / macropixel_dimensions.y };
    Coordinates picture_size_in_macropixel_units{
        m_resolution.x / macropixel_dimensions.x,
        m_resolution.y / macropixel_dimensions.y };

    for( int component_index = 0;
        component_index < m_pixel_format.m_components.size();
        component_index++ )
    {
        const Component_coding &component_coding =
            coded_pixel.m_component_codings[component_index];
        const Component &component =
            m_pixel_format.m_components[component_index];
        for( Rgba_component rgb_component = Rgba_component_red;
            rgb_component != Rgba_component_rgb_count;
            rgb_component = static_cast<Rgba_component>( rgb_component+1 ) )
        {
            const int plane_index = component_coding.m_plane_index;
            const int entry_index = component_coding.m_entry_index;
            const Plane &plane = m_pixel_format.m_planes[plane_index];
            const Entry &entry = plane.m_entries[entry_index];

            // TODO: move this fragment to separate function
            // maybe cache the result on allocation
            const int entry_rows_per_row_of_macropixels =
                plane.m_entries.size() / plane.m_entries_per_macropixel_width;
            const int entry_row = coordinates_in_macropixel_units.y *
                entry_rows_per_row_of_macropixels +
                entry_index / plane.m_entries_per_macropixel_width;
            const int bits_per_row_of_entries = std::accumulate(
                plane.m_entries.begin(),
                plane.m_entries.begin()+plane.m_entries_per_macropixel_width,
                0,
                []( int sum, const Entry &entry )
                {
                    return sum+entry.m_bit_width;
                } );
            const int offset_from_left_edge_of_macropixel = std::accumulate(
                plane.m_entries.begin(),
                plane.m_entries.begin() +
                    entry_index%plane.m_entries_per_macropixel_width,
                0,
                []( int sum, const Entry &entry )
                {
                    return sum+entry.m_bit_width;
                } );
            const int plane_offset_bits = m_plane_offsets[plane_index];
            const int start_bit =
                plane_offset_bits +
                entry_row * bits_per_row_of_entries *
                    picture_size_in_macropixel_units.x +
                bits_per_row_of_entries * coordinates_in_macropixel_units.x +
                offset_from_left_edge_of_macropixel;
            const int end_bit = start_bit + entry.m_bit_width;
            const int source_value = get_bits( m_data.data(),
                start_bit, end_bit );
            result[rgb_component] += component.m_coeff[rgb_component] *
                source_value;
        }
    }
}

