#ifndef OPENGL_DRAWER_H
#define OPENGL_DRAWER_H

#include "Display_device.h"

class OpenGL_drawer : public Display_device
{
public:
  virtual int get_bit_depth() const;
  virtual void setInputResolution();
  virtual void put_data( Rgba_component component_index,
    Coordinates position, int value );
};

#endif // OPENGL_DRAWER_H
