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
#include <yuv/Cache.h>
#include <gui/scroll_adapter.h>

#include <epoxy/gl.h>
#include <memory>

namespace YUV_tool {
/*----------------------------------------------------------------------------*/
class Drawer_gl
{
public:
    Drawer_gl();
    ~Drawer_gl();
    void initialize(Index cache_size);
    void deinitialize();
    bool is_initialized() const;
    void attach_yuv_file(Yuv_file *yuv_file);
    void draw(
            Index frame_number,
            Gdk::Rectangle visible_area);

private:
    void reallocate_buffers(Index buffers_count);
    void check_gl_errors();

    struct Resource_id
    {
        Index m_id;

        Resource_id(
                const Coordinate_pair coordinates,
                const Index zoom_level)
        {
            const Index stride = cached_power<2>(zoom_level);
            m_id =
                    get_first_child_in_n_tree_at_level<4>(0, zoom_level)
                    + coordinates.y() * stride
                    + coordinates.x();
        }

        bool operator<(const Resource_id rhs) const
        {
            return m_id < rhs.m_id;
        }
    };

    std::vector<GLuint> m_vertex_arrays;
    std::vector<GLuint> m_coordinate_buffers;
    std::vector<GLuint> m_sampling_buffers;
    std::vector<GLuint> m_textures;
    std::vector<GLuint> m_pixel_buffers;

    GLuint m_vertex_shader;
    GLuint m_fragment_shader;
    GLuint m_shader_program;

    GLint m_viewport_size_location;
    GLint m_viewport_start_location;

    std::unique_ptr<Cache<Resource_id, GLuint> > m_cache;

    Yuv_file *m_yuv_file;

    bool m_initialized;
};
/*----------------------------------------------------------------------------*/
} /* namespace YUV_tool */

#endif // DRAWER_GL_H
