#ifndef TCOMPONENTS_H
#define TCOMPONENTS_H

#include <vector>
#include <yuv/utils.h>

struct Component
{
    double R_coeff;
    double G_coeff;
    double B_coeff;
};

struct Entry
{
    int m_bit_width;
};

struct Plane
{
    // one set of entries in plane corresponds to one macropixel
    std::vector<Entry> m_entries;
    int m_entries_per_row_in_macropixel;
};

struct Component_coding
{
    int m_plane_index;
    int m_entry_index;
};

struct Coded_pixel
{
    std::vector<Component_coding> m_component_codings;
};

struct Macropixel_coding
{
    std::vector<Coded_pixel> m_coded_pixels;
    int coded_pixels_per_row_in_macropixel;
};

struct Pixel_format
{
    std::vector<Plane> m_planes;
    std::vector<Component> m_components;
    Macropixel_coding m_macropixel_coding;
};

const Pixel_format yuv_420p_8bit
{
  { // planes
    { // plane Y
      { // entries
        { 8 }, { 8 },
        { 8 }, { 8 }
      },
      2
    },
    { // plane U
      { // entries
        { 8 }
      },
      1
    },
    { // plane V
      { // entries
        { 8 }
      },
      1
    },
  },
  { // componenents
    {
      { 1.000,  1.000, 1.000 }, // Y
      { 0.000, -0.344, 1.770 }, // U
      { 1.403, -0.714, 0.000 } // V
    }
  },
  { // macropixel coding
    { // coded pixels
      { // top left
        { { 0, 0 }, { 1, 0 }, { 2, 0 } }
      },
      { // top right
        { { 0, 1 }, { 1, 0 }, { 2, 0 } }
      },
      { // bottom left
        { { 0, 2 }, { 1, 0 }, { 2, 0 } }
      },
      { // bottom right
        { { 0, 3 }, { 1, 0 }, { 2, 0 } }
      }
    },
    2
  }
};

int get_bits_per_macropixel( const Pixel_format &pixel_format );
Coordinates get_macropixel_size( const Macropixel_coding &macropixel_coding );

#endif // TCOMPONENTS_H
