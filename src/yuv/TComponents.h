#ifndef TCOMPONENTS_H
#define TCOMPONENTS_H

#include <vector>
#include <yuv/utils.h>
#include <yuv/bit_position.h>

struct Component
{
    double m_coeff[Rgba_component_rgb_count];
};

struct Entry
{
    Bit_position m_width;
};

struct Entry_row
{
    std::vector<Entry> m_entries;
};

struct Plane
{
    // one set of entries in plane corresponds to one macropixel
    std::vector<Entry_row> m_rows;
};

struct Component_coding
{
    int m_plane_index;
    int m_row_index;
    int m_entry_index;
};

struct Coded_pixel
{
    std::vector<Component_coding> m_components;
};

struct Macropixel_coding
{
    std::vector<Coded_pixel> m_pixels;
    Coordinates m_size;
};

// TODO: add support for big-endian storage
// TODO: add support for rounding up and down in case
//       picture size is not integer multiplicity of macropixel size
struct Pixel_format
{
    std::vector<Plane> m_planes;
    std::vector<Component> m_components;
    Macropixel_coding m_macropixel_coding;
};

const Pixel_format yuv_420p_8bit
{
    { // planes
        { // plane Y
            { // rows
                { { { 8 }, { 8 } } },
                { { { 8 }, { 8 } } }
            }
        },
        { // plane U
            { // rows
                { { { 8 } } }
            }
        },
        { // plane V
            { // rows
                { { { 8 } } }
            }
        },
    },
    { // componenents
        {
            { { 1.000,  1.000, 1.000 } }, // Y
            { { 0.000, -0.344, 1.770 } }, // U
            { { 1.403, -0.714, 0.000 } }  // V
        }
    },
    { // macropixel coding
        { // coded pixels
            { // top left
                { { 0, 0, 0 }, { 1, 0, 0 }, { 2, 0, 0 } }
            },
            { // top right
                { { 0, 0, 1 }, { 1, 0, 0 }, { 2, 0, 0 } }
            },
            { // bottom left
                { { 0, 1, 0 }, { 1, 0, 0 }, { 2, 0, 0 } }
            },
            { // bottom right
                { { 0, 1, 1 }, { 1, 0, 0 }, { 2, 0, 0 } }
            }
        },
        { 2, 2 }
    }
};

class Precalculated_pixel_format
{
public:
    Precalculated_pixel_format();
    void clear();
    void recalculate(const Pixel_format &pixel_format);

    const Pixel_format &get_pixel_format() const
    {
        return m_pixel_format;
    }
    int get_components_count() const
    {
        return get_pixel_format().m_components.size();
    }
    int get_planes_count() const
    {
        return m_planes.size();
    }
    int get_entry_rows_count_in_plane(const int plane_index) const
    {
        return m_planes[plane_index].m_rows.size();
    }
    int get_entry_count_in_row_in_plane(
            const int plane_index,
            const int row_index) const
    {
        return m_planes[plane_index].m_rows[row_index].m_entries.size();
    }
    Bit_position get_bits_per_macropixel() const
    {
        return m_bits_per_macropixel;
    }
    Bit_position get_bits_per_macropixel_in_plane(const int plane_index) const
    {
        return m_planes[plane_index].m_bits_per_macropixel;
    }
    Bit_position get_bits_per_entry_row_in_plane(
            const int plane_index,
            const int row_index) const
    {
        return m_planes[plane_index].m_rows[row_index].m_bits_per_macropixel;
    }
    Bit_position get_bits_per_entry(
            const int plane_index,
            const int row_index,
            const int entry_index) const
    {
        return m_pixel_format.m_planes[
                plane_index].m_rows[
                row_index].m_entries[
                entry_index].m_width;
    }
    Bit_position get_bits_per_entry(
            const Coordinates &pixel_in_macropixel,
            const int component_index) const
    {
        throw "TODO";
    }
    Bit_position get_offset(
            const Coordinates &pixel_in_macropixel,
            const Component component) const
    {
        throw "TODO";
    }
    Coordinates get_macropixel_size() const
    {
        return m_pixel_format.m_macropixel_coding.m_size;
    }
private:
    Pixel_format m_pixel_format;

    struct Entry_parameters
    {
        Bit_position m_offset;
    };

    struct Entry_row_paramters
    {
        std::vector<Entry_parameters> m_entries;
        Bit_position m_bits_per_macropixel;
    };

    struct Plane_parameters
    {
        std::vector<Entry_row_paramters> m_rows;
        Bit_position m_bits_per_macropixel;
    };

    std::vector<Plane_parameters> m_planes;

    Bit_position m_bits_per_macropixel;
};

class Precalculated_buffer_parameters : public Precalculated_pixel_format
{
public:
    Precalculated_buffer_parameters();
    void recalculate(
            const Pixel_format &format,
            const Coordinates &resolution);
    void clear();

    const Coordinates &get_resolution() const
    {
        return m_resolution;
    }
    Bit_position get_buffer_size() const
    {
        return m_buffer_size;
    }
    Bit_position get_plane_size(int plane_index) const
    {
        return m_planes[plane_index].m_size;
    }
    Bit_position get_plane_offset(int plane_index) const
    {
        return m_planes[plane_index].m_offset;
    }
    Bit_position get_macropixel_row_in_plane_size(int plane_index) const
    {
        return m_planes[plane_index].m_size_per_row_of_macropixels;
    }
    Coordinates get_size_in_macropixels() const
    {
        return m_size_in_macropixels;
    }

private:
    Coordinates m_resolution;
    Coordinates m_size_in_macropixels;

    struct Entry_row_parameters
    {
        Bit_position m_size;
    };

    struct Plane_parameters
    {
        std::vector<Entry_row_parameters> m_rows;
        Bit_position m_size_per_row_of_macropixels;
        Bit_position m_size;
        Bit_position m_offset;
    };

    std::vector<Plane_parameters> m_planes;

    Bit_position m_buffer_size;
};

Pixel_format get_expanded_pixel_format(
        const std::vector<Component> &components,
        const std::vector<Entry> &entries);
Pixel_format get_expanded_pixel_format(const Pixel_format &input);

#endif // TCOMPONENTS_H
