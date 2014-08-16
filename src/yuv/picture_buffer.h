#ifndef PICTURE_BUFFER_H
#define PICTURE_BUFFER_H

#include <yuv/utils.h>
#include <yuv/TComponents.h>

class Picture_buffer
{
public:
    Picture_buffer()
    { }
    Picture_buffer(
            const Vector<Unit::pixel> &resolution,
            const Pixel_format &pixel_format);
    ~Picture_buffer();
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
            int component_index) const;
    void set_entry(
            const Coordinates<Unit::pixel, Reference_point::picture>
                &coordinates,
            int component_index,
            int value);
    void convert_color_space(const std::vector<Component> &components);

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

#endif // PICTURE_BUFFER_H
