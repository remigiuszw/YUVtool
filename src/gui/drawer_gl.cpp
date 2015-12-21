#include <gui/drawer_gl.h>
#include <yuv/utils.h>
#include <yuv/Picture_buffer.h>
#include <yuv/Errors.h>

namespace YUV_tool {

const int tile_size = 64;
const int buffer_count = 8;

Drawer_gl::Drawer_gl() :
    m_vertex_shader(0),
    m_fragment_shader(0),
    m_shader_program(0),
    m_initialized(false)
{ }
//------------------------------------------------------------------------------
Drawer_gl::~Drawer_gl()
{
    reallocate_buffers(0);
}
//------------------------------------------------------------------------------
void Drawer_gl::initialize()
{
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
void main()
{
    vec2 scaled_position =
            vec2(
                position.x / viewport_size.x * 2.0f - 1.0f,
                position.y / viewport_size.y * 2.0f - 1.0f);
    gl_Position = vec4(position, 0.0f, 1.0f);
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
    m_initialized = true;
}
//------------------------------------------------------------------------------
void Drawer_gl::deinitialize()
{
    my_assert(m_initialized, "trying to deinitialize uninitialized Drawer_gl");
    reallocate_buffers(0);
    glDeleteProgram(m_shader_program);
    glDeleteShader(m_vertex_shader);
    glDeleteShader(m_fragment_shader);
    m_initialized = false;
}
//------------------------------------------------------------------------------
void Drawer_gl::draw(
        Yuv_file &yuv_file,
        int frame_number,
        Scroll_adapter &scroll_adapter)
{
    const Gdk::Rectangle visible_area = scroll_adapter.get_visible_area();
    const Coordinates<Unit::tile, Reference_point::picture> tiles_start
    {
        visible_area.get_x() / tile_size,
        visible_area.get_y() / tile_size
    };
    const Coordinates<Unit::tile, Reference_point::picture> tiles_end
    {
        round_up(visible_area.get_x() + visible_area.get_width(), tile_size)
                / tile_size,
        round_up(visible_area.get_y() + visible_area.get_height(), tile_size)
                / tile_size
    };
    const Vector<Unit::tile> tiles_counts
    {
        tiles_end.x() - tiles_start.x(),
        tiles_end.y() - tiles_start.y()
    };

    reallocate_buffers(buffer_count);
    const GLuint viewport_size_location =
            glGetUniformLocation(m_shader_program, "vieport_size");

    int buffer_index = 0;

    for(int tile_y = tiles_start.y(); tile_y < tiles_end.y(); tile_y++)
    {
        const Coordinates<Unit::pixel, Reference_point::picture> buffer_start{
            tiles_start.x() * tile_size,
            tile_y * tile_size};
        const Coordinates<Unit::pixel, Reference_point::picture> buffer_end{
            tiles_end.x() * tile_size,
            (tile_y + 1) * tile_size};

        const Picture_buffer coded_buffer =
                yuv_file.extract_buffer(frame_number, buffer_start, buffer_end);
        const Picture_buffer rgb_buffer =
                convert(coded_buffer, rgb_32bpp);
        const int bytes_in_GLfloat = 4;

        for(int tile_x = tiles_start.x(); tile_x < tiles_end.x(); tile_x++)
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
                // reserve storage only for RGB, not RGBA
                const int pixel_buffer_size =
                        Rgba_component_count
                        * tile_size
                        * tile_size;

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
                glTexImage2D(
                            GL_TEXTURE_2D,
                            0,
                            GL_RGB32F,
                            tile_size,
                            tile_size,
                            0,
                            GL_RGB,
                            GL_FLOAT,
                            0);
            }

            const GLfloat x0 = tile_start.x();
            const GLfloat x1 = tile_end.x();
            const GLfloat y0 = tile_start.y();
            const GLfloat y1 = tile_end.y();

            /* bind vertex array object */
            glBindVertexArray(m_vertex_arrays[buffer_index]);

            /* Buffer for generic vertex attribute 0 - coordinates */
            {
                glBindBuffer(
                            GL_ARRAY_BUFFER,
                            m_coordinate_buffers[buffer_index]);

                /* triangle strip, see OpenGL 3.3, section ?? */
                const int vertex_buffer_size = vertex_count * 2;
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

                const int texture_sampling_array_size = vertex_count * 2;
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
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 0, 0);
            }

            // TODO: draw square or triangle with m_buffer[tile_x] and
            // m_texture[tile_x]
            // glBindTexture, glTexSubImage2D, draw

            glUseProgram(m_shader_program);
            glUniform2f(
                        viewport_size_location,
                        1.0f * visible_area.get_width(),
                        1.0f * visible_area.get_height());

            glDrawArrays(GL_TRIANGLE_STRIP, 0, vertex_count);

            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_BUFFER, 0);
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            buffer_index = (buffer_index + 1) % buffer_count;
        }
    }
    reallocate_buffers( 0 );
}
//------------------------------------------------------------------------------
void Drawer_gl::reallocate_buffers(const int buffers_count)
{
    MY_ASSERT(m_vertex_arrays.size() == m_coordinate_buffers.size());
    MY_ASSERT(m_vertex_arrays.size() == m_sampling_buffers.size());
    MY_ASSERT(m_vertex_arrays.size() == m_textures.size());
    MY_ASSERT(m_vertex_arrays.size() == m_pixel_buffers.size());
    const int old_size = m_vertex_arrays.size();
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
}

} /* namespace YUV_tool */
