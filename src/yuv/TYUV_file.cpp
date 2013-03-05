#include "TYUV_file.h"

#include <iostream>

TYUV_file::TYUV_file()
{
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
}
//------------------------------------------------------------------------------
void TYUV_file::close() {
}
//------------------------------------------------------------------------------
void TYUV_file::recalculate_parameters() {
  // m_frame_size
  if( m_Pixel_format.packed )
    m_frame_size = m_res.x * m_res.y * m_Pixel_format.get_packed_bit_width()
      / m_Pixel_format.get_packed_pixel_width() / 8;
  else
    m_frame_size = m_res.x * m_res.y * m_Pixel_format.get_bits_per_pixel() / 8;
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
