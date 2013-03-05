#ifndef TCOMPONENTS_H
#define TCOMPONENTS_H

#include <string>
#include <fstream>
#include <array>

enum {
  max_packed_componenets_count = 12,// TODO: check the maximum value necessary to support worst case
  max_components_count         = 4
};

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

struct TPacked_component {
  int width; // number of bits
  int component; // number of component as in Componenets
  int offset; // offset from the leftmost pixel described in packing chunk
};

struct TPixel_format {
  bool packed; // planar vs packed
  bool interlaced; // interlaced vs progressive
  std::array<TPacked_component, max_packed_componenets_count> packing; // valid for Alignment == TAlignment::packed
  size_t packing_size;
  std::array<TComponent, max_components_count> Components;
  size_t get_packed_bit_width() const;
  size_t get_packed_pixel_width() const;
  size_t get_bits_per_pixel() const;
};

#endif // TCOMPONENTS_H
