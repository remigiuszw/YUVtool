#ifndef YUV_FILE_H
#define YUV_FILE_H

#include <yuv/Pixel_format.h>
#include <yuv/Picture_buffer.h>
#include <yuv/utils.h>

#include <boost/filesystem/fstream.hpp>
#include <string>
#include <fstream>
#include <array>

namespace YUV_tool {
/*--------------------------------------------------------------------*/
class Yuv_file
{
public:
    Yuv_file();
    Yuv_file(
            const boost::filesystem::path &path,
            const std::ios_base::openmode mode = std::ios_base::in);
    bool is_open() const;
    void open(
            const boost::filesystem::path &path,
            const std::ios_base::openmode mode = std::ios_base::in);
    void close();
    void drawFrameGL() const;
    void set_pixel_format(const Pixel_format &pixel_format);
    const Pixel_format &get_pixel_format() const;
    void set_resolution(const Vector<Unit::pixel> &resolution);
    Vector<Unit::pixel> get_resolution() const;
    Bit_position get_frame_size();
    int get_frames_count();
    void set_frames_count(const int i);
    Picture_buffer extract_buffer(
            int picture_number,
            const Coordinates<Unit::pixel, Reference_point::picture> &start,
            const Coordinates<Unit::pixel, Reference_point::picture> &end);
    void insert_buffer(
            const Picture_buffer &buffer,
            int picture_number,
            const Coordinates<Unit::pixel, Reference_point::picture> &start,
            const Coordinates<Unit::pixel, Reference_point::picture> &end);

private:
    void init_file_parameters();
    void recalculate_parameters();

    boost::filesystem::path m_path;
    boost::filesystem::fstream m_file;
    size_t m_file_size;
    Pixel_format m_pixel_format;
    Precalculated_buffer_parameters m_buffer_parameters;
    bool m_parameters_valid;
    Vector<Unit::pixel> m_resolution;
};
/*--------------------------------------------------------------------*/
} /* namespace YUV_tool */

#endif /* YUV_FILE_H */
