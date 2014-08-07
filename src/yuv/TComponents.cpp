#include <yuv/TComponents.h>

#include <stdexcept>
#include <algorithm>

//------------------------------------------------------------------------------
Pixel_format get_expanded_pixel_format(
        const std::vector<Component> &components,
        const std::vector<Entry> &entries)
{
    const int components_count = components.size();
    Pixel_format result;
    result.m_components = components;
    result.m_macropixel_coding.m_size = {1, 1};
    result.m_macropixel_coding.m_pixels.resize(1);
    Coded_pixel &target_pixel =
            result.m_macropixel_coding.m_pixels.front();
    for(int component_index = 0; component_index < components_count;
            component_index++)
    {
        Component_coding &component_coding =
                target_pixel.m_components[component_index];
        component_coding.m_plane_index = component_index;
        component_coding.m_row_index = 0;
        component_coding.m_entry_index = 0;

        Plane &plane = result.m_planes[component_index];
        plane.m_rows.resize(1);
        plane.m_rows[0].m_entries.resize(1);
        plane.m_rows[0].m_entries[0] = entries[component_index];
    }
    return result;
}
//------------------------------------------------------------------------------
Pixel_format get_expanded_pixel_format(const Pixel_format &input)
{
    const int components_count = input.m_components.size();

    std::vector<Entry> entries(components_count);
    const std::vector<Coded_pixel> &coded_pixels =
            input.m_macropixel_coding.m_pixels;
    for(int plane_index = 0; plane_index < components_count;
            plane_index++)
    {
        entries[plane_index].m_width =
                std::accumulate(
                    coded_pixels.begin(),
                    coded_pixels.end(),
                    Bit_position(0),
                    [plane_index, input](
                        const Bit_position max, const Coded_pixel &coded_pixel )
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
    }

    return get_expanded_pixel_format(input.m_components, entries);
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
                row_parameters.m_entries[entry_index].m_offset = offset;
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
        const Coordinates &resolution)
{
    Precalculated_pixel_format::recalculate(format);
    m_resolution = resolution;
    m_size_in_macropixels = resolution / get_macropixel_size();
    m_planes.resize(get_planes_count());
    Bit_position plane_offset = 0;
    for(int plane_index = 0; plane_index < get_planes_count(); plane_index++)
    {
        Plane_parameters &plane = m_planes[plane_index];
        plane.m_offset = plane_offset;
        const int rows_count = get_entry_rows_count_in_plane(plane_index);
        Bit_position bits_per_macropixel_row_in_plane = 0;
        for(int row_index = 0; row_index < rows_count; row_index++)
        {
            plane.m_rows[row_index].m_size =
                    m_size_in_macropixels.x
                    * get_bits_per_entry_row_in_plane(plane_index, row_index);
            bits_per_macropixel_row_in_plane += plane.m_rows[row_index].m_size;
        }
        plane.m_size_per_row_of_macropixels = bits_per_macropixel_row_in_plane;
        plane.m_size =
                bits_per_macropixel_row_in_plane * m_size_in_macropixels.y;
        plane_offset += plane.m_size;
    }
    m_buffer_size = plane_offset;
}
