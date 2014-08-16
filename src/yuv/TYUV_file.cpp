#include <yuv/TYUV_file.h>
#include <yuv/Errors.h>

#include <iostream>

Yuv_file::Yuv_file()
{
    m_file.exceptions(std::ios_base::badbit | std::ios_base::failbit);
}
//------------------------------------------------------------------------------
Yuv_file::Yuv_file(const boost::filesystem::path &path) :
        m_path(path)
{
    m_file.exceptions(std::ios_base::badbit | std::ios_base::failbit);
    open(path);
}
//------------------------------------------------------------------------------
bool Yuv_file::is_open() const
{
    return m_file.is_open();
}
//------------------------------------------------------------------------------
void Yuv_file::open(const boost::filesystem::path &path)
{
    std::cout << "TYUV_file::TYUV_file(const std::string file_name)\n";
    m_file.open( path );
    init_file_parameters();
}
//------------------------------------------------------------------------------
void Yuv_file::close()
{
    m_file.close();
}
//------------------------------------------------------------------------------
void Yuv_file::set_pixel_format(const Pixel_format &pixel_format)
{
    m_pixel_format = pixel_format;
}
//------------------------------------------------------------------------------
const Pixel_format &Yuv_file::get_pixel_format() const
{
    return m_pixel_format;
}
//------------------------------------------------------------------------------
void Yuv_file::set_resolution(const Vector<Unit::pixel> &resolution)
{
    m_resolution = resolution;
}
//------------------------------------------------------------------------------
Vector<Unit::pixel> Yuv_file::get_resolution() const
{
    return m_resolution;
}
//------------------------------------------------------------------------------
Bit_position Yuv_file::get_frame_size() const
{
    return m_buffer_parameters.get_buffer_size();
}
//------------------------------------------------------------------------------
int Yuv_file::get_frames_count() const
{
    return Bit_position(m_file_size, 0) / get_frame_size();
}
//------------------------------------------------------------------------------
Picture_buffer Yuv_file::extract_buffer(
        int picture_number,
        const Coordinates<Unit::pixel, Reference_point::picture> &start,
        const Coordinates<Unit::pixel, Reference_point::picture> &end)
{
    check_range(0, picture_number, get_frames_count());
    // TODO: check_range for start and end

    const Pixel_format &pixel_format = get_pixel_format();
    const Vector<Unit::pixel> buffer_size = end - start;
    Picture_buffer buffer;
    buffer.allocate(buffer_size, pixel_format);
    const Vector<Unit::pixel> macropixel_size =
            m_buffer_parameters.get_macropixel_size();
    const Vector<Unit::macropixel> buffer_size_in_macropixels =
            buffer.get_parameters().get_size_in_macropixels();
    const Vector<Unit::macropixel> picture_size_in_macropixels =
            m_buffer_parameters.get_size_in_macropixels();
    const Bit_position picture_size = get_frame_size();
    const Bit_position picture_offset = picture_number * picture_size;

    const int planes_count = pixel_format.m_planes.size();
    for(int plane_idx = 0; plane_idx < planes_count; plane_idx++)
    {
        Bit_position plane_offset =
                m_buffer_parameters.get_plane_offset(plane_idx);
        const int pixel_rows_in_macropixel_in_plane =
                m_buffer_parameters.get_entry_rows_count_in_plane(plane_idx);
        const Bit_position macropixel_row_in_plane_size_in_bits =
                m_buffer_parameters.get_macropixel_row_in_plane_size(plane_idx);
        for(int macropixel_row = 0;
            macropixel_row < buffer_size_in_macropixels.y();
            macropixel_row++)
        {
            const Bit_position macropixel_row_offset =
                    macropixel_row_in_plane_size_in_bits * macropixel_row;
            Bit_position row_in_macropixel_offset = 0;
            for(int row_in_macropixel = 0;
                row_in_macropixel < pixel_rows_in_macropixel_in_plane;
                row_in_macropixel++)
            {
                const Bit_position bits_per_entry_row_in_plane =
                        m_buffer_parameters.get_bits_per_entry_row_in_plane(
                            plane_idx, row_in_macropixel);
                const Bit_position x_position_offset =
                        start.x() / macropixel_size.x()
                        * bits_per_entry_row_in_plane;
                const Bit_position read_length =
                        buffer_size_in_macropixels.x()
                        * bits_per_entry_row_in_plane;
                const Bit_position offset =
                        picture_offset +
                        plane_offset +
                        macropixel_row_offset +
                        row_in_macropixel_offset +
                        x_position_offset;

                m_file.seekg(offset.get_bytes());
                if(offset.get_bits() != 0)
                    throw std::runtime_error(
                            "TODO: handle non-byte boundary reads");
                const Bit_position offset_in_buffer(0); // TODO
                m_file.read(reinterpret_cast<char *>(buffer.get_data().data() +
                        offset_in_buffer.get_bytes()),
                        read_length.get_bytes());

                row_in_macropixel_offset +=
                        bits_per_entry_row_in_plane *
                        picture_size_in_macropixels.x();
            }
        }
    }

//    m_file.seekg( picture_number*get_frame_size() );
//    m_file.read(reinterpret_cast<char *>( buffer.get_data().data() ),
//        get_frame_size());
    return buffer;
}
//------------------------------------------------------------------------------
void Yuv_file::init_file_parameters()
{
    // get the file size
    {
        m_file.seekg(0, std::ios_base::beg);
        auto begin = m_file.tellg();
        m_file.seekg(0, std::ios_base::end);
        auto end = m_file.tellg();
        m_file_size = end - begin;
    }
    std::cout << "m_file_size = " << m_file_size << std::endl;
}
