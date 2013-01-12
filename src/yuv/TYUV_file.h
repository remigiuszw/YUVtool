#ifndef TYUV_FILE_H
#define TYUV_FILE_H

#include <string>
#include <fstream>
#include <array>

#include "TComponents.h"

struct TXY_coords {
  int x;
  int y;
};

class TYUV_file
{
public:
            TYUV_file( const std::string file_name );
  void      drawFrameGL() const;
  void      setPixelFormat( const TPixelFormat& );
  size_t    getFramesNo();
  void      setFrameNum( size_t i );
private:
  std::string     m_file_name;
  std::ifstream   m_file;

  size_t          m_file_size;
  TPixelFormat    m_PixelFormat;
  TXY_coords      m_res;
};

#endif // TYUV_FILE_H
