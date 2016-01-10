/* 
 * Copyright 2015 Dominik Wójt
 * 
 * This file is part of YUVtool.
 * 
 * YUVtool is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * YUVtool is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with YUVtool.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef DRAWER_GL_H
#define DRAWER_GL_H

#include <yuv/Yuv_file.h>
#include <gui/scroll_adapter.h>

#include <SFML/OpenGL.hpp>

namespace YUV_tool {

class Drawer_gl
{
public:
    Drawer_gl();
    ~Drawer_gl();
    void initialize();
    void deinitialize();
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

    bool m_initialized;
};

} /* namespace YUV_tool */

#endif // DRAWER_GL_H
