#include <yuv/Pixel_format.h>

#include <stdexcept>
#include <algorithm>

namespace YUV_tool {

Pixel_format get_expanded_pixel_format(
        const Color_space &color_space,
        const std::vector<Entry> &entries)
{
    const int components_count = color_space.m_components.size();
    Pixel_format result;
    result.m_color_space = color_space;
    result.m_macropixel_coding.m_size = {1, 1};
    result.m_macropixel_coding.m_pixels.resize(1);
    Coded_pixel &target_pixel =
            result.m_macropixel_coding.m_pixels.front();
    target_pixel.m_components.resize(components_count);
    result.m_planes.resize(components_count);
    for(int component_index = 0; component_index < components_count;
            component_index++)
    {
        target_pixel.m_components[component_index] = {component_index, 0, 0};
        result.m_planes[component_index] = {{{{entries[component_index]}}}};
    }
    return result;
}
//------------------------------------------------------------------------------
Pixel_format get_expanded_pixel_format(const Pixel_format &input)
{
    const int components_count = input.m_color_space.m_components.size();

    std::vector<Entry> entries(components_count);
    const std::vector<Coded_pixel> &coded_pixels =
            input.m_macropixel_coding.m_pixels;
    for(int plane_index = 0; plane_index < components_count; plane_index++)
    {
        const Bit_position max_depth =
                std::accumulate(
                    coded_pixels.begin(),
                    coded_pixels.end(),
                    Bit_position(0),
                    [plane_index, input](
                            const Bit_position max,
                            const Coded_pixel &coded_pixel)
                    {
                        const Component_coding &input_component_coding =
                                coded_pixel.m_components[plane_index];
                        const Entry &input_entry =
                                input.m_planes[
                                input_component_coding.m_plane_index].m_rows[
                                input_component_coding.m_row_index].m_entries[
                                input_component_coding.m_entry_index];
                        return std::max(max, input_entry.m_width);
                    });
        entries[plane_index].m_width.set_position(round_up(
                max_depth.get_position(), bits_in_byte));
    }

    return get_expanded_pixel_format(input.m_color_space, entries);
}
//------------------------------------------------------------------------------
Precalculated_pixel_format::Precalculated_pixel_format()
{
    clear();
}
//------------------------------------------------------------------------------
void Precalculated_pixel_format::clear()
{
    m_bits_per_macropixel = 0;
    m_planes.clear();
    m_is_expanded = false;
}
//------------------------------------------------------------------------------
void Precalculated_pixel_format::recalculate(const Pixel_format &pixel_format)
{
    const int planes_count = pixel_format.m_planes.size();

    m_pixel_format = pixel_format;

    m_planes.resize(planes_count);
    for(int plane_index = 0; plane_index < planes_count; plane_index++)
    {
        Plane_parameters &plane_parameters = m_planes[plane_index];
        const Plane &plane = pixel_format.m_planes[plane_index];

        const int rows_count = pixel_format.m_planes[plane_index].m_rows.size();
        plane_parameters.m_rows.resize(rows_count);
        for(int row_index = 0; row_index < rows_count; row_index++)
        {
            Entry_row_paramters &row_parameters =
                    plane_parameters.m_rows[row_index];
            const Entry_row &row = plane.m_rows[row_index];

            const int entry_count = row.m_entries.size();
            row_parameters.m_entries.resize(entry_count);
            Bit_position offset = 0;
            for(int entry_index = 0; entry_index < entry_count; entry_index++)
            {
                Entry_parameters &entry = row_parameters.m_entries[entry_index];
                entry.m_offset = offset;
                entry.m_sampling_point = {-1, -1};
                entry.m_component_index = -1;
                offset += row.m_entries[entry_index].m_width;
            }
            row_parameters.m_bits_per_macropixel = offset;
        }
        plane_parameters.m_bits_per_macropixel =
                std::accumulate(
                    plane_parameters.m_rows.begin(),
                    plane_parameters.m_rows.end(),
                    Bit_position(0),
                    [](const Bit_position sum, const Entry_row_paramters &row)
                    {
                        return sum + row.m_bits_per_macropixel;
                    });
    }
    m_bits_per_macropixel =
            std::accumulate(
                m_planes.begin(),
                m_planes.end(),
                Bit_position(0),
                [](const Bit_position sum, const Plane_parameters &plane)
                {
                    return sum + plane.m_bits_per_macropixel;
                });
    for(int iy = 0; iy < get_macropixel_size().y(); iy++)
    {
        for(int ix = 0; ix < get_macropixel_size().x(); ix++)
        {
            const Coordinates<Unit::pixel, Reference_point::macropixel>
                    coordinates(ix, iy);
            const int pixel_index = iy * get_macropixel_size().y() + ix;
            for(
                    int component_index = 0;
                    component_index < get_components_count();
                    component_index++)
            {
                const Component_coding &component_coding =
                        pixel_format.m_macropixel_coding.m_pixels[
                        pixel_index].m_components[
                        component_index];
                Entry_parameters &entry_parameters =
                        m_planes[
                        component_coding.m_plane_index].m_rows[
                        component_coding.m_row_index].m_entries[
                        component_coding.m_entry_index];
                if(
                        entry_parameters.m_sampling_point
                        == Coordinates<Unit::pixel,
                            Reference_point::macropixel>(-1, -1))
                {
                    entry_parameters.m_sampling_point = coordinates;
                    entry_parameters.m_component_index = component_index;
                }
            }
        }
    }

    // m_is_expanded
    if(planes_count == get_components_count())
    {
        for(int plane_index = 0; plane_index < planes_count; plane_index++)
        {
            for(const Coded_pixel &pixel : get_pixel_format(
                    ).m_macropixel_coding.m_pixels)
            {
                const Component_coding &component =
                        pixel.m_components[plane_index];
                if(
                        component.m_plane_index != plane_index
                        || component.m_row_index != 0
                        || component.m_entry_index != 0)
                {
                    m_is_expanded = false;
                    goto is_not_expanded;
                }
            }
        }
        m_is_expanded = true;
    }
    else
    {
    is_not_expanded:
        m_is_expanded = false;
    }
}
//------------------------------------------------------------------------------
Precalculated_buffer_parameters::Precalculated_buffer_parameters()
{
    clear();
}
//------------------------------------------------------------------------------
void Precalculated_buffer_parameters::clear()
{
    Precalculated_pixel_format::clear();
    m_resolution = {0, 0};
    m_planes.clear();
    m_buffer_size = Bit_position(0);
}
//------------------------------------------------------------------------------
void Precalculated_buffer_parameters::recalculate(
        const Pixel_format &format,
        const Vector<Unit::pixel> &resolution)
{
    Precalculated_pixel_format::recalculate(format);
    m_resolution = resolution;
    m_size_in_macropixels.set(
            resolution.x() / get_macropixel_size().x(),
            resolution.y() / get_macropixel_size().y());
    m_planes.resize(get_planes_count());
    Bit_position plane_offset = 0;
    for(int plane_index = 0; plane_index < get_planes_count(); plane_index++)
    {
        Plane_parameters &plane = m_planes[plane_index];
        plane.m_offset = plane_offset;
        const int rows_count = get_entry_rows_count_in_plane(plane_index);
        plane.m_rows.resize(rows_count);
        Bit_position bits_per_macropixel_row_in_plane = 0;
        for(int row_index = 0; row_index < rows_count; row_index++)
        {
            plane.m_rows[row_index].m_size =
                    m_size_in_macropixels.x()
                    * get_bits_per_macropixel_in_row_in_plane(
                        plane_index,
                        row_index);
            plane.m_rows[row_index].m_offset =
                    bits_per_macropixel_row_in_plane;
            bits_per_macropixel_row_in_plane += plane.m_rows[row_index].m_size;
        }
        plane.m_size_per_row_of_macropixels = bits_per_macropixel_row_in_plane;
        plane.m_size =
                bits_per_macropixel_row_in_plane * m_size_in_macropixels.y();
        plane_offset += plane.m_size;
    }
    m_buffer_size = plane_offset;
}

} /* namespace YUV_tool */
