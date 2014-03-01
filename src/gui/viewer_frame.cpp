#include "viewer_frame.h"
#include <iostream>
#include <cstdlib>
#include <SFML/OpenGL.hpp>

Viewer_frame::Viewer_frame() :
    m_box( Gtk::ORIENTATION_VERTICAL ),
    m_drawing_area( sf::VideoMode( 50, 50 ) )
{
    m_action_group = Gtk::ActionGroup::create();
    m_action_group->add( Gtk::Action::create( "menu_file", "_File" ) );
    m_action_group->add( Gtk::Action::create( "quit", Gtk::Stock::QUIT ),
        sigc::mem_fun(*this, &Viewer_frame::on_action_file_quit) );
    m_action_group->add(
        Gtk::Action::create( "show_size", Gtk::Stock::ABOUT,
            "Show size of the drowing area" ),
        sigc::mem_fun( *this, &Viewer_frame::on_action_show_size ) );

    m_ui_manager = Gtk::UIManager::create();
    m_ui_manager->insert_action_group( m_action_group );
    add_accel_group( m_ui_manager->get_accel_group() );
    Glib::ustring ui_info =
        R"(
        <ui>
            <menubar name='menu_bar'>
                <menu action='menu_file'>
                    <menuitem action='show_size'/>
                    <separator/>
                    <menuitem action='quit'/>
                </menu>
            </menubar>
            <toolbar name='tool_bar'>
                <toolitem action='show_size'/>
                <toolitem action='quit'/>
            </toolbar>
        </ui>
        )";
    m_ui_manager->add_ui_from_string( ui_info );

    set_default_size( 400, 250 );

    Gtk::Widget *menu_bar = m_ui_manager->get_widget("/menu_bar");
    m_box.pack_start( *menu_bar, Gtk::PACK_SHRINK );

    Gtk::Widget *tool_bar = m_ui_manager->get_widget("/tool_bar");
    m_box.pack_start( *tool_bar, Gtk::PACK_SHRINK );

    m_drawing_area.signal_configure_event().connect(
        sigc::mem_fun( *this, &Viewer_frame::on_action_configure_event ) );
    m_drawing_area.signal_size_allocate().connect(
        sigc::mem_fun( *this, &Viewer_frame::on_action_size_allocation ) );
    m_drawing_area.signal_draw().connect(
        sigc::mem_fun( *this, &Viewer_frame::on_action_draw_event ) );
    m_scroll_adapter.add( m_drawing_area );
    m_scroll_adapter.set_internal_size( 300, 300 );
    m_box.pack_start( m_scroll_adapter, Gtk::PACK_EXPAND_WIDGET );

    add(m_box);

    show_all();
}
//------------------------------------------------------------------------------
Viewer_frame::~Viewer_frame()
{ }
//------------------------------------------------------------------------------
void Viewer_frame::on_action_file_quit()
{
    hide();
}
//------------------------------------------------------------------------------
void Viewer_frame::on_action_show_size()
{
    Gtk::MessageDialog dialog( *this, "Size of the drawing area." );

    std::stringstream ss;
    ss << "The size of the drawing area is " <<
        m_drawing_area.get_width() << 'x' <<
        m_drawing_area.get_height() << '\n';
    int internal_width, internal_height;
    m_scroll_adapter.get_internal_size( internal_width, internal_height );
    ss << "The internal size of the scrolled area is " <<
        internal_width << 'x' <<
        internal_height << '\n';
    dialog.set_secondary_text( ss.str() );

    dialog.run();
}
//------------------------------------------------------------------------------
bool Viewer_frame::on_action_configure_event( GdkEventConfigure* event )
{
    const int width = m_drawing_area.get_width();
    const int height = m_drawing_area.get_height();
    const int x0 = m_scroll_adapter.get_hadjustment()->get_value();
    const int y0 = m_scroll_adapter.get_vadjustment()->get_value();

    if( !m_drawing_area.renderWindow.setActive( true ) )
        return true;

    glViewport( 0, 0, width, height );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( x0, x0+width, y0+height, y0, -1, 1 );

    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glDisable( GL_DEPTH_TEST );

    if( !m_drawing_area.renderWindow.setActive( false ) )
        return true;

    return true;
}
//------------------------------------------------------------------------------
void Viewer_frame::on_action_size_allocation(
    Gtk::Allocation &allocation )
{
    on_action_configure_event( 0 );
}
//------------------------------------------------------------------------------
void Viewer_frame::draw_triangle()
{
    if( !m_drawing_area.renderWindow.setActive( true ) )
        return;

    glClearColor( 0.0, 0.0, 0.0, 0.0 );
    glClear( GL_COLOR_BUFFER_BIT );

    glColor3f( 1.0, 1.0, 1.0 );
    glBegin( GL_TRIANGLES );
    glVertex3f(  0.0,  0.0, 0.0 );
    glVertex3f( 20.0, 40.0, 0.0 );
    glVertex3f( 40.0,  0.0, 0.0 );
    glEnd();

//    if( gdk_gl_drawable_is_double_buffered( gldrawable ) )
//        gdk_gl_drawable_swap_buffers( gldrawable );
//    else
//        glFlush();
    glFlush();

    m_drawing_area.display();

    if( !m_drawing_area.renderWindow.setActive( false ) )
        return;
}
//------------------------------------------------------------------------------
void Viewer_frame::draw_frame()
{
    if( !m_drawing_area.renderWindow.setActive( true ) )
        return;

    glClearColor( 0.0, 0.0, 0.0, 0.0 );
    glClear( GL_COLOR_BUFFER_BIT );
    glEnable( GL_TEXTURE_2D );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );

    m_drawer_gl.draw( m_yuv_file, 0, m_scroll_adapter );

    //    if( gdk_gl_drawable_is_double_buffered( gldrawable ) )
    //        gdk_gl_drawable_swap_buffers( gldrawable );
    //    else
    //        glFlush();
    glFlush();

    if( !m_drawing_area.renderWindow.setActive( false ) )
        return;
}
//------------------------------------------------------------------------------
bool Viewer_frame::on_action_draw_event( const::Cairo::RefPtr<Cairo::Context>
  &cr )
{
    draw_triangle();
    //draw_frame();
    return true;
}

