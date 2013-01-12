#include <iostream>
#include "Errors.h"

#include "TYUV_file.h"

int main( int argc, char *argv[] ) try
{
  if( argc != 3 )
    throw( TGeneralError( "usage: yuvtool_cli input_file.yuv output_file.yuv" ) );

  TYUV_file input_file( argv[1] );

  return 0;
} catch( std::exception &e ) {
  std::cerr << "standard exception caught: " << e.what() << std::endl;
  return 1;
} catch( ... ) {
  std::cerr << "non-standard exception caught!" << std::endl;
  return 1;
}
