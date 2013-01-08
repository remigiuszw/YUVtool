#include <iostream>

#include "TYUV_file.h"

int main( int argc, char *argv[] )
{
  if( argc != 3 )
  {
    std::cout << "usage: yuvtool_cli input_file.yuv output_file.yuv\n";
    return 1;
  }

  TYUV_file input_file( argv[1] );



  return 0;
}
