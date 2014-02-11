#include <gtk/gtkgl.h>
#include <cassert>

#include <gui/drawer_gl.h>
#include <yuv/utils.h>
#include <yuv/picture_buffer.h>

const int tile_size = 64;

Drawer_gl::Drawer_gl()
{
}
//------------------------------------------------------------------------------
Drawer_gl::~Drawer_gl()
{
    reallocate_buffers( 0 );
}
//------------------------------------------------------------------------------
void Drawer_gl::draw( Yuv_file &yuv_file, int frame_number,
    Gtk::DrawingArea &drawing_area, Scroll_adapter &scroll_adapter )
{
    const Gdk::Rectangle visible_area = scroll_adapter.get_visible_area();
    const Coordinates tiles_start
    {
        visible_area.get_x()/tile_size,
        visible_area.get_y()/tile_size
    };
    const Coordinates tiles_end
    {
        round_up( visible_area.get_x()+visible_area.get_width(), tile_size ) /
            tile_size,
        round_up( visible_area.get_y()+visible_area.get_height(), tile_size ) /
            tile_size
    };
    const Coordinates tiles_counts
    {
        tiles_end.x - tiles_start.x,
        tiles_end.y - tiles_start.y
    };
    reallocate_buffers( tiles_counts.x );

    Picture_buffer picture_buffer;
    yuv_file.extract_picture( picture_buffer, frame_number );

    for( int tile_y=tiles_start.y; tile_y<tiles_end.y; tile_y++ )
    {
        for( int tile_x=tiles_start.x; tile_x<tiles_end.x; tile_x++ )
        {
            glBindBuffer( GL_PIXEL_UNPACK_BUFFER, m_buffers[tile_x] );
            // glBufferData - to be sure storage is assigned and operations on
            // old data are not going to stall us
            // reserve storage only for RGB, not RGBA
            glBufferData( GL_PIXEL_UNPACK_BUFFER,
                static_cast<int>(Rgba_component::rgb_count) * tile_size *
                tile_size, 0, GL_STREAM_DRAW );
            Byte *mapped_buffer = static_cast<Byte *>(
                glMapBuffer( GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY ) );
            if( !mapped_buffer )
                throw std::runtime_error( "mapping of an OpenGL buffer failed"
                    );
            const Coordinates tile_start
            {
                tile_x*tile_size,
                tile_y*tile_size
            };
            const Coordinates tile_end
            {
                (tile_x+1)*tile_size,
                (tile_y+1)*tile_size
            };
            picture_buffer.fill_tile_rgb( tile_start, tile_end, mapped_buffer );
        }
    }
}
//------------------------------------------------------------------------------
void Drawer_gl::reallocate_buffers( int buffers_count )
{
    assert( m_buffers.size() == m_textures.size() );
    const int old_size = m_buffers.size();
    if( old_size != buffers_count )
    {
        if( old_size < buffers_count )
        {
            m_buffers.resize( buffers_count );
            m_textures.resize( buffers_count );
            glGenBuffers( buffers_count-old_size, &m_buffers[old_size] );
            glGenTextures( buffers_count-old_size, &m_textures[old_size] );
        }
        else
        {
            glDeleteBuffers( old_size-buffers_count,
                &m_buffers[buffers_count] );
            glDeleteTextures( old_size-buffers_count,
                &m_textures[buffers_count] );
            m_buffers.resize( buffers_count );
            m_textures.resize( buffers_count );
        }
    }
}
