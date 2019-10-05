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
#ifndef PICTURE_BUFFER_H
#define PICTURE_BUFFER_H

#include <yuv/utils.h>
#include <yuv/Pixel_format.h>

namespace YUV_tool {

class Picture_buffer
{
public:
    Picture_buffer()
    { }
    Picture_buffer(
            const Vector<Unit::pixel> &resolution,
            const Pixel_format &pixel_format);
    void allocate(
            const Vector<Unit::pixel> &resolution,
            const Pixel_format &pixel_format);
    const Pixel_format &get_pixel_format() const;
    const Vector<Unit::pixel> &get_resolution() const;
    const Precalculated_buffer_parameters &get_parameters() const;
    std::vector< Byte > &get_data();
    const std::vector< Byte > &get_data() const;
//    void fill_tile_rgb(Coordinates tile_start, Coordinates tile_end,
//        Byte *data ) const;
    int get_entry(
            const Coordinates<Unit::pixel, Reference_point::picture>
                &coordinates,
            Index component_index) const;
    void set_entry(
            const Coordinates<Unit::pixel, Reference_point::picture>
                &coordinates,
            Index component_index,
            int value);
    void convert_color_space(const Color_space &color_space);

private:
//    void draw_macropixel( Coordinates coordinates, Byte *data ) const;
//    void draw_pixel( Coordinates coordinates, const Coded_pixel &coded_pixel,
//        double (&result)[Rgba_component_rgb_count] ) const;

    Precalculated_buffer_parameters m_parameters;
    std::vector< Byte > m_data;
};

Picture_buffer convert(
        const Picture_buffer &source,
        const Pixel_format &pixel_format);
Picture_buffer expand_sampling(const Picture_buffer &source);
Picture_buffer subsample(
        const Picture_buffer &source,
        const Pixel_format &pixel_format);

} /* namespace YUV_tool */

#endif // PICTURE_BUFFER_H
