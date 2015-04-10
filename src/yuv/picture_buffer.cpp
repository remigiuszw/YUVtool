#include <algorithm>
#include <stdexcept>
#include <Eigen/Dense>

#include "picture_buffer.h"
#include "Errors.h"

namespace
{
int get_bits(const Byte *data, const Bit_position start, const Bit_position end)
{
    // TODO: optimize!!!
    int result = 0;
    int mask = 1;
    for(Bit_position i = start; i < end; ++i)
    {
        const int bit = (data[i.get_bytes()] >> i.get_bits()) & 1;
        result |= bit ? mask : 0;
        mask <<= 1;
    }
    return result;
}
//------------------------------------------------------------------------------
void set_bits(Byte *data, const Bit_position start, const Bit_position end,
        const int value)
{
    // TODO: optimize!!!
    int mask = 1;
    for(Bit_position i = start; i < end; ++i)
    {
        const int bit = value & mask ? 1 : 0;
        data[i.get_bytes()] |= bit << i.get_bits();
        mask <<= 1;
    }
}
}
//------------------------------------------------------------------------------
Picture_buffer::Picture_buffer(
        const Vector<Unit::pixel> &resolution,
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
        const Vector<Unit::pixel> &resolution,
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
const Vector<Unit::pixel> &Picture_buffer::get_resolution() const
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
//void Picture_buffer::fill_tile_rgb(Coordinates tile_start,
//    Coordinates tile_end, Byte *data) const
//{
//    // data is assumed to be in form:
//    // format: GL_RGB
//    // type: GL_BYTE
//    // as given to glTexImage2D

//    const int output_bytes_per_pixel = 3;

//    const Coordinates macropixel_size = m_parameters.get_macropixel_size();
//    //const int bytes_per_pixel_component = 1;
//    Coordinates macropixel_coordinates;

//    for( macropixel_coordinates.y=tile_start.y;
//         macropixel_coordinates.y<tile_end.y;
//         macropixel_coordinates.y+=macropixel_size.y )
//    {
//        for( macropixel_coordinates.x=tile_start.x;
//             macropixel_coordinates.x<tile_end.x;
//             macropixel_coordinates.x+=macropixel_size.x )
//        {
//            const int tile_width = tile_end.x - tile_start.x;
//            const int pixel_number_in_tile =
//                tile_width * macropixel_coordinates.y +
//                macropixel_coordinates.x;
//            draw_macropixel( macropixel_coordinates,
//                data + output_bytes_per_pixel * pixel_number_in_tile );
//        }
//    }
//}
//------------------------------------------------------------------------------
//void Picture_buffer::draw_macropixel( Coordinates coordinates, Byte *data )
//    const
//{
//    const Coordinates size = m_parameters.get_macropixel_size();
//    Coordinates pixel_offset;
//    for( pixel_offset.y=0;
//        pixel_offset.y<size.y;
//        pixel_offset.y++ )
//    {
//        for( pixel_offset.x=0;
//            pixel_offset.x<size.x;
//            pixel_offset.x++ )
//        {
//            double rgb_values[Rgba_component_rgb_count] = { 0 };
//            const Coded_pixel &coded_pixel =
//                get_pixel_format().m_macropixel_coding.m_pixels[
//                    pixel_offset.y*size.x+pixel_offset.x];
//            draw_pixel( coordinates+pixel_offset, coded_pixel, rgb_values );
//        }
//    }
//}
//------------------------------------------------------------------------------
int Picture_buffer::get_entry(
        const Coordinates<Unit::pixel, Reference_point::picture> &coordinates,
        const int component_index) const
{
    Bit_position start =
            get_parameters().get_entry_offset(coordinates, component_index);
    Bit_position width =
            get_parameters().get_bits_per_entry(coordinates, component_index);
    return get_bits(get_data().data(), start, start+width);
}
//------------------------------------------------------------------------------
void Picture_buffer::set_entry(
        const Coordinates<Unit::pixel, Reference_point::picture> &coordinates,
        const int component_index,
        const int value)
{
    Bit_position start =
            get_parameters().get_entry_offset(coordinates, component_index);
    Bit_position width =
            get_parameters().get_bits_per_entry(coordinates, component_index);
    return set_bits(get_data().data(), start, start+width, value);
}
//------------------------------------------------------------------------------
void Picture_buffer::convert_color_space(
        const Color_space &color_space)
{
    my_assert(
            m_parameters.is_expanded(),
            "color space conversion of not expanded pixel formats is not "
            "supported");
    if(color_space == m_parameters.get_pixel_format().m_color_space)
        return;
    const auto &input_components =
            m_parameters.get_pixel_format().m_color_space.m_components;
    const auto &output_components =
            color_space.m_components;
    const int input_components_count = input_components.size();
    const int output_components_count = output_components.size();

    Eigen::Matrix4d input_matrix;
    Eigen::Matrix4d output_matrix;
    for(int i = 0; i < Rgba_component_count; i++)
    {
        for(int j = 0; j < Rgba_component_count; j++)
        {
            if(i < input_components_count)
                input_matrix(i, j) = input_components[i].m_coeff[j];
            else
                input_matrix(i, j) = (i == j) ? 1 : 0;
            if(i < output_components_count)
                output_matrix(i, j) = output_components[i].m_coeff[j];
            else
                output_matrix(i, j) = (i == j) ? 1 : 0;
        }
    }
    Eigen::Matrix4d combined_matrix =
            output_matrix * input_matrix.inverse();

    /* TODO: combine all the operations into one matrix multiplication and one
     * vector addition */

    for(const auto &xy : m_parameters.get_pixel_range())
    {
        Eigen::Vector4d input;
        for(int i = 0; i < Rgba_component_count; i++)
        {
            if(i >= input_components_count)
            {
                input(i) = 1;
                continue;
            }
            const int quantized_input = get_entry(xy, i);
            const int input_width =
                    m_parameters.get_bits_per_entry(xy, i).get_position();
            const Component &input_component = input_components[i];
            const double (&valid_range)[2] = input_component.m_valid_range;
            const double (&encoded_range)[2] = input_component.m_encoded_range;
            const double input_in_encoded_range =
                    static_cast<double>(quantized_input)
                    / (1 << input_width);
            const double input_in_0_to_1 =
                    (input_in_encoded_range - encoded_range[0])
                    / (encoded_range[1] - encoded_range[0]);
            input[i] =
                    input_in_0_to_1 * (valid_range[1] - valid_range[0])
                    + valid_range[0];
        }
        Eigen::Vector4d output = combined_matrix * input;
        for(int i = 0; i < output_components_count; i++)
        {
            const Component &output_component = output_components[i];
            const double (&valid_range)[2] = output_component.m_valid_range;
            const double (&encoded_range)[2] = output_component.m_encoded_range;
            const int output_in_0_to_1 =
                    (output[i] - valid_range[0])
                    / (valid_range[1] - valid_range[0]);
            const int output_in_encoded_range =
                    output_in_0_to_1 * (encoded_range[1] - encoded_range[0])
                    + encoded_range[0];
            const int output_width =
                    m_parameters.get_bits_per_entry(xy, i).get_position();
            const int quantized_output =
                    output_in_encoded_range * (1 << output_width);
            set_entry(xy, i, quantized_output);
        }
    }
}
//------------------------------------------------------------------------------
//void Picture_buffer::draw_pixel( Coordinates coordinates,
//    const Coded_pixel &coded_pixel,
//    double (&result)[Rgba_component_rgb_count] ) const
//{
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
//}
//------------------------------------------------------------------------------
Picture_buffer convert(
        const Picture_buffer &source,
        const Pixel_format &pixel_format)
{
    Picture_buffer expanded = expand_sampling(source);
    expanded.convert_color_space(pixel_format.m_color_space);
    return subsample(expanded, pixel_format);
}
//------------------------------------------------------------------------------
Picture_buffer expand_sampling(const Picture_buffer &source)
{
    const Precalculated_buffer_parameters &source_parameters =
            source.get_parameters();
    const Vector<Unit::pixel> macropixel_size =
            source_parameters.get_macropixel_size();
    const Pixel_format expanded_format =
            get_expanded_pixel_format(source_parameters.get_pixel_format());
    Picture_buffer expanded(source.get_resolution(), expanded_format);
    const Precalculated_buffer_parameters &expanded_parameters =
            expanded.get_parameters();

    const Vector<Unit::macropixel> size_in_macropixels =
            source_parameters.get_size_in_macropixels();

    my_assert(
            cast_to_pixels(size_in_macropixels, macropixel_size)
            == source.get_resolution(),
            "not supported yet");

    for(int iy = 0; iy < size_in_macropixels.y(); iy++)
    {
        for(int jy = 0; jy < macropixel_size.y(); jy++)
        {
            for(int ix = 0; ix < size_in_macropixels.x(); ix++)
            {
                for(int jx = 0; jx < macropixel_size.x(); jx++)
                {
                    const Coordinates<Unit::pixel, Reference_point::picture>
                            coordinates(
                                ix * macropixel_size.x() + jx,
                                iy * macropixel_size.y() + jy);
                    const int components_count =
                            source_parameters.get_components_count();
                    for(
                            int component_index = 0;
                            component_index < components_count;
                            component_index++)
                    {
                        const Bit_position input_bitdepth =
                                source_parameters.get_bits_per_entry(
                                    Coordinates<Unit::pixel,
                                        Reference_point::macropixel>(jx, jy),
                                    component_index);
                        const Bit_position output_bitdepth =
                                expanded_parameters.get_bits_per_entry(
                                    Coordinates<Unit::pixel,
                                        Reference_point::macropixel>(0, 0),
                                    component_index);
                        const int input_value =
                                source.get_entry(coordinates, component_index);
                        const int output_value =
                                input_value
                                << (
                                    output_bitdepth.get_position()
                                    - input_bitdepth.get_position());
                        expanded.set_entry(
                                coordinates,
                                component_index,
                                output_value);
                    }
                }
            }
        }
    }
    return expanded;
}
//------------------------------------------------------------------------------
Picture_buffer subsample(
        const Picture_buffer &source,
        const Pixel_format &pixel_format)
{
    Picture_buffer subsampled(source.get_resolution(), pixel_format);
    const Precalculated_buffer_parameters &source_parameters =
            source.get_parameters();
    const Precalculated_buffer_parameters &subsampled_parameters =
            subsampled.get_parameters();
    const Vector<Unit::pixel> macropixel_size =
            subsampled_parameters.get_macropixel_size();
    const Vector<Unit::macropixel> size_in_macropixels =
            subsampled_parameters.get_size_in_macropixels();
    my_assert(
            source_parameters.is_expanded(),
            "subsampling subsampled format is not supported");
    my_assert(
            cast_to_pixels(size_in_macropixels, macropixel_size)
            != source.get_resolution(),
            "not supported yet");
    const int planes_count = subsampled_parameters.get_planes_count();

    for(int plane_index = 0; plane_index < planes_count; plane_index++)
    {
        for(int my = 0; my < size_in_macropixels.y(); my++)
        {
            const int entry_rows_count_in_plane =
                    subsampled_parameters.get_entry_rows_count_in_plane(
                        plane_index);
            for(
                    int row_index = 0;
                    row_index < entry_rows_count_in_plane;
                    row_index++)
            {
                for(int mx = 0; mx < size_in_macropixels.x(); mx++)
                {
                    const int entries_in_row_count =
                        subsampled_parameters.get_entry_count_in_row_in_plane(
                                plane_index,
                                row_index);
                    const Coordinates<
                            Unit::macropixel,
                            Reference_point::picture> macropixel(mx, my);
                    for(
                            int entry_index = 0;
                            entry_index < entries_in_row_count;
                            entry_index++)
                    {
                        const Coordinates<
                                Unit::pixel,
                                Reference_point::macropixel> sampling_point =
                                    subsampled_parameters.get_sampling_point(
                                        plane_index,
                                        row_index,
                                        entry_index);
                        const Coordinates<
                                Unit::pixel,
                                Reference_point::picture> coordinates =
                                    cast_to_pixels(
                                        macropixel,
                                        macropixel_size,
                                        sampling_point);
                        const int component_index =
                                subsampled_parameters.get_sampled_component(
                                    plane_index,
                                    row_index,
                                    entry_index);
                        const Bit_position input_bitdepth =
                                source_parameters.get_bits_per_entry(
                                    Coordinates<
                                        Unit::pixel,
                                        Reference_point::macropixel>(0, 0),
                                    component_index);
                        const Bit_position output_bitdepth =
                                subsampled_parameters.get_bits_per_entry(
                                    sampling_point,
                                    component_index);
                        const int input_value =
                                source.get_entry(coordinates, component_index);
                        const int output_value =
                                input_value
                                << (
                                    output_bitdepth.get_position()
                                    - input_bitdepth.get_position());
                        subsampled.set_entry(
                                coordinates,
                                component_index,
                                output_value);
                    }
                }
            }
        }
    }
    return subsampled;
}
