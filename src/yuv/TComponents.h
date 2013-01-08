#ifndef TCOMPONENTS_H
#define TCOMPONENTS_H

#include <string>
#include <fstream>
#include <array>

enum class TAlignment {
  planar,
  packed
};

struct TComponent {
  int bit_depth;
  double R_coeff;
  double G_coeff;
  double B_coeff;
};

struct TPackedComponent {
  int width; // number of bits
  int component; // number of component as in Componenets
  int offset; // offset from the leftmost pixel described in packing chunk
};

struct TPixelFormat {
  bool packed; // planar vs packed
  bool interleased; // interleased vs progressive
  std::array<TPackedComponent, 6> packing; // valid for Alignment == TAlignment::packed, TODO: check the maximum value necessary to support worst case
  size_t Packing_size;
  std::array<TComponent,4> Components;
};

#endif // TCOMPONENTS_H
