#ifndef TYUV_FILE_H
#define TYUV_FILE_H

#include <string>
#include <fstream>

struct TXY_corrds {
  int x;
  int y;
};

struct TComponent {
  int bit_depth;
  double Y_coeff;
  double U_coeff;
  double V_coeff;
};

struct TComponents {

};

class TYUV_file
{
public:
  TYUV_file( const std::string file_name );
private:
  std::ifstream   file;

  int             length;
  TXY_coorrds     res;
};

#endif // TYUV_FILE_H
