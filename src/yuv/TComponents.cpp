#include "TComponents.h"

size_t TPixel_format::get_packed_bit_width() const {
  size_t result = 0;
  for( size_t i=0; i<packing_size; i++ )
    result += packing[i].width;
  return result;
}

size_t TPixel_format::get_packed_pixel_width() const {
  int result = 0;
  for( size_t i=0; i<packing_size; i++ )
    result = std::max( result, packing[i].offset );
  return result;
}

size_t TPixel_format::get_bits_per_pixel() const {
  size_t result = 0;
  for( TComponent Component : Components )
    result += Component.bit_depth;
  return result;
}
