#ifndef TYUV_FILE_H
#define TYUV_FILE_H

#include <string>
#include <fstream>
#include <array>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "utils.h"
#include "TComponents.h"

class Yuv_file {

public:
  Yuv_file();
  Yuv_file( const boost::filesystem::path &path );
  bool      is_open() const;
  void      open( const boost::filesystem::path &path );
  void      close();
  void      recalculate_parameters();
  void      drawFrameGL() const;
  void      set_pixel_format( const Pixel_format &pixel_format );
  void      set_resolution( Coordinates resolution );
  Coordinates get_resolution() const;
  size_t    get_frame_size() const;
  size_t    get_frames_count() const;
  void      set_frame_number( size_t i ) const;

private:
  void      init_file_parameters();

  boost::filesystem::path m_path;
  boost::filesystem::ifstream m_file;
  size_t          m_file_size;
  size_t          m_frame_size;
  Pixel_format    m_pixel_format;
  Coordinates     m_resolution;
};

#endif // TYUV_FILE_H
