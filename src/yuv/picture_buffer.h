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

private:
    std::vector< Byte > m_data;
    Coordinates m_resolution;
    Pixel_format m_pixel_format;
};

#endif // PICTURE_BUFFER_H
