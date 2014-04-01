#ifndef DRAWER_GL_H
#define DRAWER_GL_H

#include <yuv/TYUV_file.h>
#include <gui/scroll_adapter.h>
#include <GL/gl.h>

class Drawer_gl
{
public:
    Drawer_gl();
    ~Drawer_gl();
    void draw( Yuv_file &yuv_file, int frame_number,
        Scroll_adapter &scroll_adapter );

private:
    void reallocate_buffers( int buffers_count );

    //const int m_tile_size = 64;
    std::vector<GLuint> m_buffers;
    std::vector<GLuint> m_textures;
};

#endif // DRAWER_GL_H
