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
#include <gui/drawer_gl.h>
#include <yuv/utils.h>
#include <yuv/Picture_buffer.h>
#include <yuv/Errors.h>

namespace YUV_tool {

const Index tile_size = 64;
const Index buffer_count = 8;
const Index tile_size_in_bytes =
        tile_size * tile_size * Rgba_component_count * 2;

Drawer_gl::Drawer_gl() :
    m_vertex_shader(0),
    m_fragment_shader(0),
    m_shader_program(0),
    m_yuv_file(nullptr),
    m_initialized(false)
{ }
//------------------------------------------------------------------------------
Drawer_gl::~Drawer_gl()
{
    if(m_initialized)
        deinitialize();
}
/*----------------------------------------------------------------------------*/
/* This function is required to be called after OpenGL context is ready
 * and before any drawing is done using the drawer.
 * cache_size is expressed in number of 64x64 pixel tiles. Each tile occupies
 * 32 KB of video memory. */
void Drawer_gl::initialize(const Index cache_size)
{
    my_assert(!m_initialized, "trying to initialize initialized drawer");
    GLint success;
    const int max_log_length = 512;
    GLchar info_log[max_log_length];

    /* vetex shader */
    m_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    MY_ASSERT(m_vertex_shader);
    const GLchar *vertex_shader_source =
R"(
#version 330 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 texture_sampling;
out vec2 texture_sampling2;
uniform vec2 viewport_size;
uniform vec2 viewport_start;
void main()
{
    vec2 shifted_position =
            position - viewport_start;
    vec2 scaled_position =
            vec2(
                shifted_position.x / viewport_size.x * 2.0f - 1.0f,
                shifted_position.y / viewport_size.y * -2.0f + 1.0f);
    gl_Position = vec4(scaled_position, 0.0f, 1.0f);
    texture_sampling2 = texture_sampling;
}
)";
    glShaderSource(m_vertex_shader, 1, &vertex_shader_source, 0);
    glCompileShader(m_vertex_shader);
    glGetShaderiv(m_vertex_shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(m_vertex_shader, max_log_length, 0, info_log);
        my_assert(
                    false,
                    std::string("could not compile vertex shader: ")
                    + info_log);
    }

    /* fragment shader */
    m_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    MY_ASSERT(m_fragment_shader);
    const GLchar *fragment_shader_source =
R"(
#version 330 core
in vec2 texture_sampling2;
out vec4 color;
uniform sampler2D my_texture;
void main()
{
    color = texture(my_texture, texture_sampling2);
}
)";
    glShaderSource(m_fragment_shader, 1, &fragment_shader_source, 0);
    glCompileShader(m_fragment_shader);
    glGetShaderiv(m_fragment_shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(m_fragment_shader, max_log_length, 0, info_log);
        my_assert(
                    false,
                    std::string("could not compile fragment shader: ")
                    + info_log);
    }

    /* shader program */
    m_shader_program = glCreateProgram();
    MY_ASSERT(m_shader_program);
    glAttachShader(m_shader_program, m_vertex_shader);
    glAttachShader(m_shader_program, m_fragment_shader);
    glLinkProgram(m_shader_program);
    glGetProgramiv(m_shader_program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(m_shader_program, max_log_length, 0, info_log);
        my_assert(
                    false,
                    std::string("could not link shader program: ")
                    + info_log);
    }

    m_viewport_size_location =
            glGetUniformLocation(m_shader_program, "viewport_size");
    m_viewport_start_location =
            glGetUniformLocation(m_shader_program, "viewport_start");
    my_assert(
                m_viewport_size_location != -1
                && m_viewport_start_location != -1,
                "GL could not find location of uniform");

    m_initialized = true;
    check_gl_errors();
}
/*----------------------------------------------------------------------------*/
/* This function should be called when no more drawing is expected to be done */
void Drawer_gl::deinitialize()
{
    my_assert(m_initialized, "trying to deinitialize uninitialized Drawer_gl");
    reallocate_buffers(0);
    glDeleteProgram(m_shader_program);
    glDeleteShader(m_vertex_shader);
    glDeleteShader(m_fragment_shader);
    m_initialized = false;

    check_gl_errors();
}
/*----------------------------------------------------------------------------*/
bool Drawer_gl::is_initialized() const
{
    return m_initialized;
}
/*----------------------------------------------------------------------------*/
/* yuv_file might be nullptr in no yuv_file is to be attached */
void Drawer_gl::attach_yuv_file(Yuv_file *yuv_file)
{
    m_yuv_file = yuv_file;
}
/*----------------------------------------------------------------------------*/
void Drawer_gl::draw(
        Index frame_number,
        const Gdk::Rectangle visible_area)
{
    my_assert(is_initialized(), "drawer cannot draw before initialization");
    my_assert(m_yuv_file, "cannot draw when no YUV file is attached to drawer");

    glClearColor(0.3f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    const Coordinates<Unit::pixel, Reference_point::picture>
            visible_area_begin(visible_area.get_x(), visible_area.get_y());
    const Coordinates<Unit::pixel, Reference_point::picture>
            visible_area_end(
                visible_area.get_x() + visible_area.get_width(),
                visible_area.get_y() + visible_area.get_height());
    const Coordinates<Unit::tile, Reference_point::picture> tiles_start
    {
        visible_area_begin.x() / tile_size,
        visible_area_begin.y() / tile_size
    };
    //    const Coordinates<Unit::tile, Reference_point::picture> tiles_end
    //    {
    //        round_up(visible_area_end.x(), tile_size) / tile_size,
    //        round_up(visible_area_end.y(), tile_size) / tile_size
    //    };
    const Coordinates<Unit::tile, Reference_point::picture> tiles_end{1, 1};

    const Vector<Unit::tile> tiles_counts
    {
        tiles_end.x() - tiles_start.x(),
        tiles_end.y() - tiles_start.y()
    };

    reallocate_buffers(buffer_count);

    Index buffer_index = 0;

    for(Index tile_y = tiles_start.y(); tile_y < tiles_end.y(); tile_y++)
    {
        const Coordinates<Unit::pixel, Reference_point::picture> buffer_start{
            tiles_start.x() * tile_size,
            tile_y * tile_size};
        const Coordinates<Unit::pixel, Reference_point::picture> buffer_end{
            tiles_end.x() * tile_size,
            (tile_y + 1) * tile_size};

        const Picture_buffer coded_buffer =
                m_yuv_file->extract_buffer(
                    frame_number,
                    buffer_start,
                    buffer_end);
        const Picture_buffer rgb_buffer =
                convert(coded_buffer, rgb_32bpp);
        const Index bytes_in_GLfloat = 4;

        for(Index tile_x = tiles_start.x(); tile_x < tiles_end.x(); tile_x++)
        {
            const int vertex_count = 4;
            const Coordinates<Unit::pixel, Reference_point::picture>
                    tile_start{
                        tile_x * tile_size,
                        0};
            const Coordinates<Unit::pixel, Reference_point::picture>
                    tile_end{
                        (tile_x + 1) * tile_size,
                        tile_size};

            /* bind and fill the pixel buffer */
            {
                glBindBuffer(
                            GL_PIXEL_UNPACK_BUFFER,
                            m_pixel_buffers[buffer_index]);
                // glBufferData - to be sure storage is assigned and operations on
                // old data are not going to stall us
                // reserve storage for RGBA
                const Index pixel_buffer_size =
                        Rgba_component_count
                        * tile_size
                        * tile_size;

                /* TODO: extract 64x64 tile from the picture wide slice in
                 * rgb_buffer */

                glBufferData(
                            GL_PIXEL_UNPACK_BUFFER,
                            pixel_buffer_size,
                            rgb_buffer.get_data().data(),
                            GL_STREAM_DRAW);
            }

            /* bind texture and provide it with data from the currently bound
             * pixel unpack buffer */
            {
                glBindTexture(GL_TEXTURE_2D, m_textures[buffer_index]);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

                glTexImage2D(
                            GL_TEXTURE_2D,
                            0,
                            GL_RGB,
                            tile_size,
                            tile_size,
                            0,
                            GL_RGB,
                            GL_UNSIGNED_BYTE,
                            0);
            }

            const GLfloat x0 = tile_start.x();
            const GLfloat x1 = tile_end.x();
            const GLfloat y0 = tile_start.y() + tile_y * tile_size;
            const GLfloat y1 = tile_end.y() + tile_y * tile_size;

            /* bind vertex array object */
            glBindVertexArray(m_vertex_arrays[buffer_index]);

            /* Buffer for generic vertex attribute 0 - coordinates */
            {
                glBindBuffer(
                            GL_ARRAY_BUFFER,
                            m_coordinate_buffers[buffer_index]);

                /* triangle strip, see OpenGL 3.3, section ?? */
                const Index vertex_buffer_size = vertex_count * 2;
                GLfloat vertex_array[vertex_buffer_size] =
                {
                    x0, y0,
                    x0, y1,
                    x1, y0,
                    x1, y1
                };

                glBufferData(
                            GL_ARRAY_BUFFER,
                            vertex_buffer_size * bytes_in_GLfloat,
                            vertex_array,
                            GL_STATIC_DRAW);

                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
            }

            /* Buffer for generic vertex attribute 1 - texture sampling points
             */
            {
                glBindBuffer(GL_ARRAY_BUFFER, m_sampling_buffers[buffer_index]);

                const Index texture_sampling_array_size = vertex_count * 2;
                GLfloat texture_sampling_array[texture_sampling_array_size] =
                {
                    0.0, 0.0,
                    0.0, 1.0,
                    1.0, 0.0,
                    1.0, 1.0
                };

                glBufferData(
                            GL_ARRAY_BUFFER,
                            texture_sampling_array_size * bytes_in_GLfloat,
                            texture_sampling_array,
                            GL_STATIC_DRAW);

                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
            }

            glUseProgram(m_shader_program);
            glUniform2f(
                        m_viewport_size_location,
                        1.0f * visible_area.get_width(),
                        1.0f * visible_area.get_height());
            glUniform2f(
                        m_viewport_start_location,
                        1.0f * visible_area.get_x(),
                        1.0f * visible_area.get_y());

            glDrawArrays(GL_TRIANGLE_STRIP, 0, vertex_count);

            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            buffer_index = (buffer_index + 1) % buffer_count;

            check_gl_errors();
        }
    }
    reallocate_buffers( 0 );
}
/*----------------------------------------------------------------------------*/
void Drawer_gl::reallocate_buffers(const Index buffers_count)
{
    MY_ASSERT(m_vertex_arrays.size() == m_coordinate_buffers.size());
    MY_ASSERT(m_vertex_arrays.size() == m_sampling_buffers.size());
    MY_ASSERT(m_vertex_arrays.size() == m_textures.size());
    MY_ASSERT(m_vertex_arrays.size() == m_pixel_buffers.size());
    const Index old_size = m_vertex_arrays.size();
    if(old_size != buffers_count)
    {
        if(old_size < buffers_count)
        {
            m_vertex_arrays.resize(buffers_count);
            m_coordinate_buffers.resize(buffers_count);
            m_sampling_buffers.resize(buffers_count);
            m_pixel_buffers.resize(buffers_count);
            m_textures.resize(buffers_count);
            glGenVertexArrays(
                        buffers_count - old_size,
                        &m_vertex_arrays[old_size]);
            glGenBuffers(
                        buffers_count - old_size,
                        &m_coordinate_buffers[old_size]);
            glGenBuffers(
                        buffers_count - old_size,
                        &m_sampling_buffers[old_size]);
            glGenTextures(buffers_count - old_size, &m_textures[old_size]);
            glGenBuffers(buffers_count - old_size, &m_pixel_buffers[old_size]);
        }
        else
        {
            glDeleteVertexArrays(
                    old_size - buffers_count,
                    &m_vertex_arrays[buffers_count]);
            glDeleteBuffers(
                    old_size - buffers_count,
                    &m_coordinate_buffers[buffers_count]);
            glDeleteBuffers(
                    old_size - buffers_count,
                    &m_sampling_buffers[buffers_count]);
            glDeleteTextures(
                    old_size - buffers_count,
                    &m_textures[buffers_count]);
            glDeleteBuffers(
                    old_size - buffers_count,
                    &m_pixel_buffers[buffers_count]);
            m_vertex_arrays.resize(buffers_count);
            m_coordinate_buffers.resize(buffers_count);
            m_sampling_buffers.resize(buffers_count);
            m_pixel_buffers.resize(buffers_count);
            m_textures.resize(buffers_count);
        }
    }
    check_gl_errors();
}
/*----------------------------------------------------------------------------*/
void Drawer_gl::check_gl_errors()
{
    GLenum error_code = glGetError();

    while(error_code != GL_NO_ERROR)
    {
        const char *error_name = 0;
        switch(error_code)
        {
        case GL_INVALID_ENUM:
            error_name = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error_name = "GL_INVALID_ENUM";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error_name = "GL_INVALID_ENUM";
            break;
        case GL_OUT_OF_MEMORY:
            error_name = "GL_INVALID_ENUM";
            break;
        default:
            error_name = "unknown error";
            break;
        }

        my_assert(false, "GL error detected: " + std::string(error_name));
    }
}
/*----------------------------------------------------------------------------*/
} /* namespace YUV_tool */
