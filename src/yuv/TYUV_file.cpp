#include <yuv/TYUV_file.h>

#include <iostream>\

#include <yuv/Errors.h>

Yuv_file::Yuv_file() {
  m_file.exceptions( std::ios_base::badbit | std::ios_base::failbit );
}
//------------------------------------------------------------------------------
Yuv_file::Yuv_file( const boost::filesystem::path &path ) :
  m_path( path )
{
  m_file.exceptions( std::ios_base::badbit | std::ios_base::failbit );
  open( path );
}
//------------------------------------------------------------------------------
bool Yuv_file::is_open() const {
  return m_file.is_open();
}
//------------------------------------------------------------------------------
void Yuv_file::open( const boost::filesystem::path &path ) {
  std::cout << "TYUV_file::TYUV_file(const std::string file_name)\n";
  m_file.open( path );
  init_file_parameters();
}
//------------------------------------------------------------------------------
void Yuv_file::close() {
  m_file.close();
}
//------------------------------------------------------------------------------
void Yuv_file::recalculate_parameters() {
  // m_frame_size
  m_frame_size = m_resolution.x * m_resolution.y * get_bits_per_macropixel(
    m_pixel_format ) /
      m_pixel_format.m_macropixel_coding.m_coded_pixels.size() / bits_in_byte;
}
//------------------------------------------------------------------------------
void Yuv_file::set_pixel_format(const Pixel_format &pixel_format )
{
    m_pixel_format = pixel_format;
}
//------------------------------------------------------------------------------
const Pixel_format &Yuv_file::get_pixel_format() const
{
    return m_pixel_format;
}
//------------------------------------------------------------------------------
void Yuv_file::set_resolution( Coordinates resolution ) {
  m_resolution = resolution;
}
//------------------------------------------------------------------------------
Coordinates Yuv_file::get_resolution() const {
  return m_resolution;
}
//------------------------------------------------------------------------------
int Yuv_file::get_frame_size() const {
  return m_frame_size;
}
//------------------------------------------------------------------------------
int Yuv_file::get_frames_count() const {
  return m_file_size/get_frame_size();
}
//------------------------------------------------------------------------------
void Yuv_file::extract_picture( Picture_buffer &picture_buffer,
    int picture_number )
{
    check_range( 0, picture_number, get_frames_count() );
    picture_buffer.allocate( get_resolution(), get_pixel_format() );
    m_file.seekg( picture_number*get_frame_size() );
    m_file.read( reinterpret_cast<char *>( picture_buffer.get_data().data() ),
        get_frame_size() );
}
//------------------------------------------------------------------------------
void Yuv_file::init_file_parameters() {
  // get the file size
  {
    auto begin = m_file.tellg();
    m_file.seekg( 0, std::ios_base::end );
    auto end   = m_file.tellg();
    m_file_size = end - begin;
  }
  std::cout << "m_file_size = " << m_file_size << std::endl;
}
