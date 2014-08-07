#include <algorithm>
#include <stdexcept>

#include "picture_buffer.h"

Picture_buffer::Picture_buffer(
        const Coordinates &resolution,
        const Pixel_format &pixel_format)
{
    allocate(resolution, pixel_format);
}
//------------------------------------------------------------------------------
Picture_buffer::~Picture_buffer()
{
}
//------------------------------------------------------------------------------
void Picture_buffer::allocate(
        const Coordinates &resolution,
        const Pixel_format &pixel_format)
{
    m_parameters.recalculate(pixel_format, resolution);
    m_data.resize(m_parameters.get_buffer_size().get_bytes());
}
//------------------------------------------------------------------------------
const Pixel_format &Picture_buffer::get_pixel_format() const
{
    return m_parameters.get_pixel_format();
}
//------------------------------------------------------------------------------
const Coordinates &Picture_buffer::get_resolution() const
{
    return m_parameters.get_resolution();
}
//------------------------------------------------------------------------------
const Precalculated_buffer_parameters &Picture_buffer::get_parameters() const
{
    return m_parameters;
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

    const Coordinates macropixel_size = m_parameters.get_macropixel_size();
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
    const Coordinates size = m_parameters.get_macropixel_size();
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
                get_pixel_format().m_macropixel_coding.m_pixels[
                    pixel_offset.y*size.x+pixel_offset.x];
            draw_pixel( coordinates+pixel_offset, coded_pixel, rgb_values );
        }
    }
}
//------------------------------------------------------------------------------
int Picture_buffer::get_entry(
        const Component_coding &component_coding,
        const Coordinates &coordinates_in_macropixels) const
{
    throw std::runtime_error("TODO");
}
//------------------------------------------------------------------------------
void Picture_buffer::set_entry(
        int value,
        const Component_coding &Component_coding,
        const Coordinates &coordinates_in_macropixels)
{
    throw std::runtime_error("TODO");
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
//    const Coordinates macropixel_size = m_parameters.get_macropixel_size();
//    const Coordinates coordinates_in_macropixel_units =
//            coordinates / macropixel_size;
//    const Coordinates picture_size_in_macropixel_units =
//            m_parameters.get_size_in_macropixels();

//    const int components_count = m_parameters.get_components_count();
//    for(int component_index = 0;
//        component_index < components_count;
//        component_index++)
//    {
//        const Component_coding &component_coding =
//            coded_pixel.m_components[component_index];
//        const Component &component =
//            m_pixel_format.m_components[component_index];
//        for( Rgba_component rgb_component = Rgba_component_red;
//            rgb_component != Rgba_component_rgb_count;
//            rgb_component = static_cast<Rgba_component>( rgb_component+1 ) )
//        {
//            const int plane_index = component_coding.m_plane_index;
//            const int entry_index = component_coding.m_entry_index;
//            const Plane &plane = m_pixel_format.m_planes[plane_index];
//            const Entry &entry = plane.m_entries[entry_index];

//            // TODO: move this fragment to separate function
//            // maybe cache the result on allocation
//            const int entry_rows_per_row_of_macropixels =
//                plane.m_entries.size() / plane.m_entries_per_macropixel_width;
//            const int entry_row = coordinates_in_macropixel_units.y *
//                entry_rows_per_row_of_macropixels +
//                entry_index / plane.m_entries_per_macropixel_width;
//            const int bits_per_row_of_entries = std::accumulate(
//                plane.m_entries.begin(),
//                plane.m_entries.begin()+plane.m_entries_per_macropixel_width,
//                0,
//                []( int sum, const Entry &entry )
//                {
//                    return sum+entry.m_bit_width;
//                } );
//            const int offset_from_left_edge_of_macropixel = std::accumulate(
//                plane.m_entries.begin(),
//                plane.m_entries.begin() +
//                    entry_index%plane.m_entries_per_macropixel_width,
//                0,
//                []( int sum, const Entry &entry )
//                {
//                    return sum+entry.m_bit_width;
//                } );
//            const int plane_offset_bits = m_plane_offsets[plane_index];
//            const int start_bit =
//                plane_offset_bits +
//                entry_row * bits_per_row_of_entries *
//                    picture_size_in_macropixel_units.x +
//                bits_per_row_of_entries * coordinates_in_macropixel_units.x +
//                offset_from_left_edge_of_macropixel;
//            const int end_bit = start_bit + entry.m_bit_width;
//            const int source_value = get_bits( m_data.data(),
//                start_bit, end_bit );
//            result[rgb_component] += component.m_coeff[rgb_component] *
//                source_value;
//        }
//    }
}
//------------------------------------------------------------------------------
Picture_buffer convert(
        const Picture_buffer &source,
        const Pixel_format &pixel_format)
{
    Picture_buffer expanded = expand_sampling(source);
    throw std::runtime_error("TODO");
    return expanded;
}
//------------------------------------------------------------------------------
Picture_buffer expand_sampling(const Picture_buffer &source)
{
    Precalculated_buffer_parameters source_parameters =
            source.get_parameters();
    const Coordinates macropixel_size =
            source_parameters.get_macropixel_size();
    const Pixel_format expanded_format =
            get_expanded_pixel_format(source_parameters.get_pixel_format());
    Picture_buffer expanded(source.get_resolution(), expanded_format);
    const Macropixel_coding &macropixel_coding =
            source_parameters.get_pixel_format().m_macropixel_coding;

    const Coordinates size_in_macropixels =
            source_parameters.get_size_in_macropixels();
    if(size_in_macropixels * macropixel_size != source.get_resolution())
        throw std::runtime_error("not supported yet");

    for(int iy = 0; iy < size_in_macropixels.y; iy++)
    {
        for(int jy = 0; jy < macropixel_size.y; jy++)
        {
            for(int ix = 0; ix < size_in_macropixels.x; ix++)
            {
                for(int jx = 0; jx < macropixel_size.x; jx++)
                {
                    const Coordinates coordinates{
                        ix * macropixel_size.x + jx,
                        iy * macropixel_size.y + jy};
                    const int pixel_index = jy * macropixel_size.y + jx;
                    const Coded_pixel &coded_pixel =
                            macropixel_coding.m_pixels[pixel_index];
                    const int components_count =
                            source_parameters.get_components_count();
                    for(    int component_index = 0;
                            component_index < components_count;
                            component_index++)
                    {
                        const Component_coding &component_coding =
                                coded_pixel.m_components[component_index];
                        const Bit_position input_bitdepth =
                                source_parameters.get_bits_per_entry(
                                    Coordinates{jx, jy}, component_index);
                        const Bit_position output_bitdepth =
                                expanded_format.m_planes[
                                component_index].m_rows[0].m_entries[
                                0].m_width;
                        const int input_value = source.get_entry(
                                component_coding,
                                Coordinates{ix, iy});
                        const int output_value =
                                input_value <<
                                (
                                    output_bitdepth.get_position()
                                    - input_bitdepth.get_position());
                        expanded.set_entry(
                                output_value,
                                Component_coding{component_index, 0},
                                coordinates);
                    }
                }
            }
        }
    }
    return expanded;
}
