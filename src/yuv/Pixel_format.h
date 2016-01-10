/* 
 * Copyright 2015 Dominik Wójt
 * 
 * This file is part of YUVtool.
 * 
 * YUVtool is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * YUVtool is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with YUVtool.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef PIXEL_FORMAT_H
#define PIXEL_FORMAT_H

#include <yuv/utils.h>
#include <yuv/Bit_position.h>
#include <yuv/Coordinates.h>

#include <vector>
#include <algorithm>

namespace YUV_tool {

struct Component
{
    /*
     * E'_X = K_XR * E'_R + K_XG * E'_G + K_XB * E'_B + K_XA * E'_A
     */
    double m_coeff[Rgba_component_count];
    double m_valid_range[2];
    double m_encoded_range[2];
};

inline bool operator==(const Component &a, const Component &b)
{
    return
            std::equal(
                a.m_coeff, a.m_coeff + Rgba_component_count,
                b.m_coeff)
            && std::equal(
                a.m_valid_range, a.m_valid_range + 2,
                b.m_valid_range)
            && std::equal(
                a.m_encoded_range, a.m_encoded_range + 2,
                b.m_encoded_range);
}

struct Color_space
{
    std::vector<Component> m_components;
};

inline bool operator==(const Color_space &a, const Color_space &b)
{
    return a.m_components == b.m_components;
}

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
    Vector<Unit::pixel> m_size;
};

// TODO: add support for big-endian storage
// TODO: add support for rounding up and down in case
//       picture size is not integer multiplicity of macropixel size
struct Pixel_format
{
    std::vector<Plane> m_planes;
    Color_space m_color_space;
    Macropixel_coding m_macropixel_coding;
};

// full range RGB

const Color_space RGB_color_space
{
    {
        { // R
            { 1, 0, 0, 0 },
            { 0, 1 },
            { 0, 1 }
        },
        { // G
            { 0, 1, 0, 0 },
            { 0, 1 },
            { 0, 1 }
        },
        { // B
            { 0, 0, 1, 0 },
            { 0, 1 },
            { 0, 1 }
        }
    }
};

// studio RGB

const Color_space sRGB_color_space
{
    {
        { // R
            { 1, 0, 0, 0 },
            { 0.0, 1.0 },
            { 16.0/256, 235.0/256 }
        },
        { // G
            { 0, 1, 0, 0 },
            { 0.0, 1.0 },
            { 16.0/256, 235.0/256 }
        },
        { // B
            { 0, 0, 1, 0 },
            { 0.0, 1.0 },
            { 16.0/256, 235.0/256 }
        }
    }
};

// ITU BT.601 standard
const double ITU601_K_R = 0.299;
const double ITU601_K_B = 0.114;

const Color_space ITU601_YCbCr_color_space
{
    {
        { // Y
            {
                ITU601_K_R,
                1 - ITU601_K_B - ITU601_K_R,
                ITU601_K_B,
                0
            },
            { 0.0, 1.0 },
            { 16.0/256, 235.0/256 }
        },
        { // Cb
            {
                -ITU601_K_R/(2*(1-ITU601_K_B)),
                -(1-ITU601_K_B-ITU601_K_R)/(2*(1-ITU601_K_B)),
                0.5,
                0
            },
            { -0.5, 0.5 },
            { 16.0/256, 240.0/256 }
        },
        { // Cr
            {
                0.5,
                -(1-ITU601_K_B-ITU601_K_R)/(2*(1-ITU601_K_R)),
                -ITU601_K_B/(2*(1-ITU601_K_R)),
                0
            },
            { -0.5, 0.5 },
            { 16.0/256, 240.0/256 }
        }
    }
};

// ITU BT.709 standard
const double ITU709_K_R = 0.2126;
const double ITU709_K_B = 0.0722;

const Color_space ITU709_YCbCr_color_space
{
    {
        { // Y
            {
                ITU709_K_R,
                1 - ITU709_K_B - ITU709_K_R,
                ITU709_K_B,
                0
            },
            { 0.0, 1.0 },
            { 16.0/256, 235.0/256 }
        },
        { // Cb
            {
                -ITU709_K_R/(2*(1-ITU709_K_B)),
                -(1-ITU709_K_B-ITU709_K_R)/(2*(1-ITU709_K_B)),
                0.5,
                0
            },
            { -0.5, 0.5 },
            { 16.0/256, 240.0/256 }
        },
        { // Cr
            {
                0.5,
                -(1-ITU709_K_B-ITU709_K_R)/(2*(1-ITU709_K_R)),
                -ITU709_K_B/(2*(1-ITU709_K_R)),
                0
            },
            { -0.5, 0.5 },
            { 16.0/256, 240.0/256 }
        }
    }
};

// ITU BT.2020 standard
const double ITU2020_K_R = 0.2627;
const double ITU2020_K_B = 0.0593;

const Color_space ITU2020_YCbCr_color_space
{
    {
        { // Y
            {
                ITU2020_K_R,
                1 - ITU2020_K_B - ITU2020_K_R,
                ITU2020_K_B,
                0
            },
            { 0.0, 1.0 },
            { 16.0/256, 235.0/256 }
        },
        { // Cb
            {
                -ITU2020_K_R/(2*(1-ITU2020_K_B)),
                -(1-ITU2020_K_B-ITU2020_K_R)/(2*(1-ITU2020_K_B)),
                0.5,
                0
            },
            { -0.5, 0.5 },
            { 16.0/256, 240.0/256 }
        },
        { // Cr
            {
                0.5,
                -(1-ITU2020_K_B-ITU2020_K_R)/(2*(1-ITU2020_K_R)),
                -ITU2020_K_B/(2*(1-ITU2020_K_R)),
                0
            },
            { -0.5, 0.5 },
            { 16.0/256, 240.0/256 }
        }
    }
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
    ITU601_YCbCr_color_space,
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

const Pixel_format rgb_32bpp
{
    { // planes
        { // plane RGB
            { // rows
                { {// entries
                    { 8 }, { 8 }, { 8 }, { 8 }
                } }
            }
        }
    },
    RGB_color_space,
    { // macropixel coding
        { // coded pixels
            { // coded pixel
                { { 0, 0, 0 }, { 0, 0, 1 }, { 0, 0, 2 } }
            }
        },
        { 1, 1 }
    }
};

class Precalculated_pixel_format
{
private:
    Pixel_format m_pixel_format;

    struct Entry_parameters
    {
        Bit_position m_offset;
        Coordinates<Unit::pixel, Reference_point::macropixel> m_sampling_point;
        int m_component_index;
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
    bool m_is_expanded;

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
        return m_pixel_format.m_color_space.m_components.size();
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
    Bit_position get_bits_per_macropixel_in_row_in_plane(
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
            const Coordinates<Unit::pixel, Reference_point::macropixel>
                &pixel_in_macropixel,
            const int component_index) const
    {
        const Component_coding &coding =
                m_pixel_format.m_macropixel_coding.m_pixels[
                get_pixel_coding_index(pixel_in_macropixel)].m_components[
                component_index];
        return
                m_pixel_format.m_planes[
                    coding.m_plane_index].m_rows[
                    coding.m_row_index].m_entries[
                    coding.m_entry_index].m_width;
    }
    Bit_position get_entry_offset_in_macropixel_in_row_in_plane(
            const int plane_index,
            const int row_index,
            const int entry_index) const
    {
        return
                m_planes[
                plane_index].m_rows[
                row_index].m_entries[
                entry_index].m_offset;
    }
    Vector<Unit::pixel> get_macropixel_size() const
    {
        return m_pixel_format.m_macropixel_coding.m_size;
    }
    bool is_expanded() const
    {
        return m_is_expanded;
    }
    Coordinates<Unit::pixel, Reference_point::macropixel> get_sampling_point(
            const int plane_index,
            const int row_index,
            const int entry_index) const
    {
        return m_planes[plane_index].m_rows[row_index].m_entries[
                entry_index].m_sampling_point;
    }
    int get_sampled_component(
            const int plane_index,
            const int row_index,
            const int entry_index) const
    {
        return m_planes[plane_index].m_rows[row_index].m_entries[
                entry_index].m_component_index;
    }
private:
    int get_pixel_coding_index(
            const Coordinates<Unit::pixel, Reference_point::macropixel>
                &pixel_in_macropixel) const
    {
        return
                pixel_in_macropixel.y() * get_macropixel_size().x()
                + pixel_in_macropixel.x();
    }
};

class Precalculated_buffer_parameters : public Precalculated_pixel_format
{
public:
    Precalculated_buffer_parameters();
    void recalculate(
            const Pixel_format &format,
            const Vector<Unit::pixel> &resolution);
    void clear();

    const Vector<Unit::pixel> &get_resolution() const
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
    Vector<Unit::macropixel> get_size_in_macropixels() const
    {
        return m_size_in_macropixels;
    }
    Bit_position get_entry_offset(
            const Coordinates<Unit::pixel, Reference_point::picture>
                &coordinates,
            const int component_index) const
    {
        Coordinates<Unit::pixel, Reference_point::macropixel>
                pixel_in_macropixel;
        const Coordinates<Unit::macropixel, Reference_point::picture>
                macropixel_coordintes =
                    cast_to_macropixels(
                        coordinates,
                        get_macropixel_size(),
                        pixel_in_macropixel);
        const Macropixel_coding &macropixel_coding =
                get_pixel_format().m_macropixel_coding;
        const Coded_pixel &coded_pixel =
                macropixel_coding.m_pixels[
                    macropixel_coding.m_size.x() * pixel_in_macropixel.y()
                    + pixel_in_macropixel.x()];
        const Component_coding &component_coding =
                coded_pixel.m_components[component_index];
        const int plane_index = component_coding.m_plane_index;
        const Plane_parameters &plane_parameters =
                m_planes[component_coding.m_plane_index];
        const Bit_position entry_offset_in_row_of_pixels_in_macropixel =
                get_entry_offset_in_macropixel_in_row_in_plane(
                    plane_index,
                    component_coding.m_row_index,
                    component_coding.m_entry_index);
        return
                plane_parameters.m_offset
                + (
                    plane_parameters.m_size_per_row_of_macropixels
                    * macropixel_coordintes.y())
                + plane_parameters.m_rows[pixel_in_macropixel.y()].m_offset
                + (
                    macropixel_coordintes.x()
                    * get_bits_per_macropixel_in_row_in_plane(
                        plane_index,
                        component_coding.m_row_index))
                + entry_offset_in_row_of_pixels_in_macropixel;
    }
    using Precalculated_pixel_format::get_bits_per_entry;
    Bit_position get_bits_per_entry(
            const Coordinates<Unit::pixel, Reference_point::picture>
                &coordinates,
            const int component_index) const
    {
        Coordinates<Unit::pixel, Reference_point::macropixel>
                pixel_in_macropixel;
        cast_to_macropixels(
                coordinates,
                get_macropixel_size(),
                pixel_in_macropixel);
        return get_bits_per_entry(pixel_in_macropixel, component_index);
    }
    Coordinate_range<Unit::pixel, Reference_point::picture> get_pixel_range(
            ) const
    {
        return Coordinate_range<Unit::pixel, Reference_point::picture>(
                Coordinates<Unit::pixel, Reference_point::picture>(0, 0),
                m_resolution);
    }

private:
    Vector<Unit::pixel> m_resolution;
    Vector<Unit::macropixel> m_size_in_macropixels;

    struct Entry_row_parameters
    {
        Bit_position m_size;
        Bit_position m_offset;
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

Pixel_format get_expanded_pixel_format(const Color_space &color_space,
        const std::vector<Entry> &entries);
Pixel_format get_expanded_pixel_format(const Pixel_format &input);

} /* namespace YUV_tool */

#endif /* PIXEL_FORMAT_H */
