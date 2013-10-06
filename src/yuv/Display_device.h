#ifndef DISPLAY_DEVICE_H
#define DISPLAY_DEVICE_H

#include "utils.h"

class Display_device {
public:
  virtual int get_bit_depth() const = 0;
  virtual void setInputResolution() = 0;
  virtual void put_data( Rgba_component component_index,
    Coordinates position, int value ) = 0;
};

#endif // DISPLAY_DEVICE_H
