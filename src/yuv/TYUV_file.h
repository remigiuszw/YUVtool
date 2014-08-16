#ifndef TYUV_FILE_H
#define TYUV_FILE_H

#include <string>
#include <fstream>
#include <array>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <yuv/utils.h>
#include <yuv/TComponents.h>
#include <yuv/picture_buffer.h>

class Yuv_file
{
public:
    Yuv_file();
    Yuv_file(const boost::filesystem::path &path);
    bool is_open() const;
    void open(const boost::filesystem::path &path);
    void close();
    void recalculate_parameters();
    void drawFrameGL() const;
    void set_pixel_format(const Pixel_format &pixel_format);
    const Pixel_format &get_pixel_format() const;
    void set_resolution(const Vector<Unit::pixel> &resolution);
    Vector<Unit::pixel> get_resolution() const;
    Bit_position get_frame_size() const;
    int get_frames_count() const;
    Picture_buffer extract_buffer(
            int picture_number,
            const Coordinates<Unit::pixel, Reference_point::picture> &start,
            const Coordinates<Unit::pixel, Reference_point::picture> &end);

private:
    void init_file_parameters();

    boost::filesystem::path m_path;
    boost::filesystem::ifstream m_file;
    size_t m_file_size;
    Pixel_format m_pixel_format;
    Precalculated_buffer_parameters m_buffer_parameters;
    Vector<Unit::pixel> m_resolution;
};

#endif // TYUV_FILE_H
