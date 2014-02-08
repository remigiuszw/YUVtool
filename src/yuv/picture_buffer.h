#ifndef PICTURE_BUFFER_H
#define PICTURE_BUFFER_H

#include <yuv/utils.h>
#include <yuv/TComponents.h>

class Picture_buffer
{
public:
    Picture_buffer();
    ~Picture_buffer();
    void allocate( const Coordinates &resolution,
        const Pixel_format &pixel_format );
    const Pixel_format &get_pixel_format() const;
    Coordinates get_resolution() const;
    std::vector< Byte > &get_data();
    const std::vector< Byte > &get_data() const;
    void fill_tile_rgb(Coordinates tile_start, Coordinates tile_end,
        Byte *data ) const;

private:
    void draw_macropixel( Coordinates coordinates ) const;
    void draw_pixel( Coordinates coordinates, const Coded_pixel &coded_pixel,
        double (&data)[static_cast<int>(Rgba_component::rgb_count)] ) const;

    std::vector< Byte > m_data;
    Coordinates m_resolution;
    Pixel_format m_pixel_format;

    // precomputed values from m_pixel_format and m_resolution
    std::vector<int> m_plane_offsets;
    std::vector<int> m_row_strides;
};

#endif // PICTURE_BUFFER_H
