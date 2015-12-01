#ifndef DRAWER_GL_H
#define DRAWER_GL_H

#include <yuv/TYUV_file.h>
#include <gui/scroll_adapter.h>
#include <SFML/OpenGL.hpp>

namespace YUV_tool {

class Drawer_gl
{
public:
    Drawer_gl();
    ~Drawer_gl();
    void initialize();
    void draw(
            Yuv_file &yuv_file,
            int frame_number,
            Scroll_adapter &scroll_adapter);

private:
    void reallocate_buffers(int buffers_count);

    //const int m_tile_size = 64;
    std::vector<GLuint> m_vertex_arrays;
    std::vector<GLuint> m_coordinate_buffers;
    std::vector<GLuint> m_sampling_buffers;
    std::vector<GLuint> m_textures;
    std::vector<GLuint> m_pixel_buffers;

    GLuint m_vertex_shader;
    GLuint m_fragment_shader;
    GLuint m_shader_program;
};

} /* namespace YUV_tool */

#endif // DRAWER_GL_H
