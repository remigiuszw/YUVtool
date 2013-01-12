#include "TYUV_file.h"

#include <iostream>

TYUV_file::TYUV_file(const std::string file_name) :
  m_file_name( file_name ),
  m_file( m_file_name, std::ios_base::binary | std::ios_base::in )
{
  m_file.exceptions( std::ios_base::badbit | std::ios_base::failbit );
  std::cout << "TYUV_file::TYUV_file(const std::string file_name)\n";
  // get the file size
  {
    auto begin = m_file.tellg();
    m_file.seekg( 0, std::ios_base::end );
    auto end   = m_file.tellg();
    m_file_size = end - begin;
  }
  std::cout << "m_file_size = " << m_file_size << std::endl;
}
