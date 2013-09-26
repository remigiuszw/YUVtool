#include "TYUV_file.h"

#include <iostream>

TYUV_file::TYUV_file() {
  m_file.exceptions( std::ios_base::badbit | std::ios_base::failbit );
}
//------------------------------------------------------------------------------
TYUV_file::TYUV_file( const boost::filesystem::path &path ) :
  m_path( path )
{
  m_file.exceptions( std::ios_base::badbit | std::ios_base::failbit );
  open( path );
}
//------------------------------------------------------------------------------
bool TYUV_file::is_open() const {
  return m_file.is_open();
}
//------------------------------------------------------------------------------
void TYUV_file::open( const boost::filesystem::path &path ) {
  std::cout << "TYUV_file::TYUV_file(const std::string file_name)\n";
  m_file.open( path );
  init_file_parameters();
}
//------------------------------------------------------------------------------
void TYUV_file::close() {
  m_file.close();
}
//------------------------------------------------------------------------------
void TYUV_file::recalculate_parameters() {
  const int bits_in_byte = 8;
  // m_frame_size
  m_frame_size = m_resolution.x * m_resolution.y * get_bits_per_macropixel(
    m_pixel_format ) /
      m_pixel_format.m_macropixel_coding.m_coded_pixels.size() / bits_in_byte;
}
//------------------------------------------------------------------------------
void TYUV_file::set_pixel_format(const Pixel_format &pixel_format ) {
  m_pixel_format = pixel_format;
}
//------------------------------------------------------------------------------
void TYUV_file::set_resolution( TXY_coords resolution ) {
  m_resolution = resolution;
}
//------------------------------------------------------------------------------
TXY_coords TYUV_file::get_resolution() const {
  return m_resolution;
}
//------------------------------------------------------------------------------
size_t TYUV_file::get_frame_size() const {
  return m_frame_size;
}
//------------------------------------------------------------------------------
size_t TYUV_file::get_frames_count() const {
  return m_file_size/get_frame_size();
}
//------------------------------------------------------------------------------
void TYUV_file::init_file_parameters() {
  // get the file size
  {
    auto begin = m_file.tellg();
    m_file.seekg( 0, std::ios_base::end );
    auto end   = m_file.tellg();
    m_file_size = end - begin;
  }
  std::cout << "m_file_size = " << m_file_size << std::endl;
}
