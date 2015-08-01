#include "viewer_frame.h"
#include <iostream>
#include <cstdlib>
#include <SFML/OpenGL.hpp>

Viewer_frame::Viewer_frame() :
    m_box( Gtk::ORIENTATION_VERTICAL ),
    m_drawing_area( sf::VideoMode( 50, 50 ) )
{
    m_action_group = Gtk::ActionGroup::create();
    m_action_group->add(
        Gtk::Action::create( "menu_file", "_File" ) );
    m_action_group->add(
        Gtk::Action::create( "open_file", Gtk::Stock::OPEN ),
        sigc::mem_fun( *this, &Viewer_frame::on_action_file_open ) );
    m_action_group->add(
        Gtk::Action::create( "close_file", Gtk::Stock::CLOSE ),
        sigc::mem_fun( *this, &Viewer_frame::on_action_file_close ) );
    m_action_group->add(
        Gtk::Action::create( "quit", Gtk::Stock::QUIT ),
        sigc::mem_fun( *this, &Viewer_frame::on_action_file_quit ) );
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
                    <menuitem action='open_file'/>
                    <menuitem action='close_file'/>
                    <separator/>
                    <menuitem action='show_size'/>
                    <separator/>
                    <menuitem action='quit'/>
                </menu>
            </menubar>
            <toolbar name='tool_bar'>
                <toolitem action='open_file'/>
                <toolitem action='close_file'/>
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

    m_drawing_area.signal_size_allocate().connect(
        sigc::mem_fun( *this, &Viewer_frame::on_action_size_allocation ) );
    m_drawing_area.signal_draw().connect(
        sigc::mem_fun( *this, &Viewer_frame::on_action_draw_event ) );

    //m_scroll_adapter.add(m_dummy_button );
    m_scroll_adapter.add(m_drawing_area);
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

    const Gtk::Allocation allocation =
            m_drawing_area.get_allocation();
    ss
        << "The drawing area allocation is "
        << allocation.get_x() << 'x' << allocation.get_y() << " + "
        << allocation.get_width() << 'x' << allocation.get_height() << '\n';

    int internal_width, internal_height;
    m_scroll_adapter.get_internal_size( internal_width, internal_height );
    ss << "The internal size of the scrolled area is " <<
        internal_width << 'x' <<
        internal_height << '\n';

    const Gdk::Rectangle visible_area =
            m_scroll_adapter.get_visible_area();
    ss
        << "The visible area rectangle is "
        << visible_area.get_x() << 'x' << visible_area.get_y() << " + "
        << visible_area.get_width() << 'x' << visible_area.get_height() << '\n';

    dialog.set_secondary_text( ss.str() );

    dialog.run();
}
//------------------------------------------------------------------------------
void Viewer_frame::on_action_file_open()
{
    Gtk::FileChooserDialog dialog( *this, "Choose YUV file.",
        Gtk::FILE_CHOOSER_ACTION_OPEN );

    dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
    dialog.add_button("Select", Gtk::RESPONSE_OK);

    const int result = dialog.run();

    switch( result )
    {
    case Gtk::RESPONSE_OK:
        try
        {
            m_yuv_file.open( dialog.get_filename() );
        }
        catch( std::runtime_error & )
        {
            std::cerr << "failed to open file: " << dialog.get_filename();
        }
        break;
    case Gtk::RESPONSE_CANCEL:
        break;
    default:
        std::cerr << "unknown responce of file chooser dialog\n";
        break;
    }
}
//------------------------------------------------------------------------------
void Viewer_frame::on_action_file_close()
{
    if( m_yuv_file.is_open() )
    {
        m_yuv_file.close();
    }
    else
    {
        std::cerr << "tried to close file, while none is open\n";
    }
}
//------------------------------------------------------------------------------
void Viewer_frame::on_action_size_allocation(
    Gtk::Allocation &allocation )
{
    const Gdk::Rectangle visible_area =
            m_scroll_adapter.get_visible_area();
    const int width = visible_area.get_width();
    const int height = visible_area.get_height();
    const int x0 = visible_area.get_x();
    const int y0 = visible_area.get_y();
    int total_width;
    int total_height;
    m_scroll_adapter.get_internal_size(total_width, total_height);

    if( !m_drawing_area.renderWindow.setActive( true ) )
        return;

    glViewport(0, 0, width, height);

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( x0, x0+width, y0+height, y0, -1, 1 );

    glClearColor( 0.0f, 0.5f, 0.0f, 0.0f );
    glDisable( GL_DEPTH_TEST );

    if( !m_drawing_area.renderWindow.setActive( false ) )
        return;
}
//------------------------------------------------------------------------------
void Viewer_frame::draw_triangle()
{
    if( !m_drawing_area.renderWindow.setActive( true ) )
        return;

    glClearColor( 0.0, 0.0, 0.5, 0.0 );
    glClear( GL_COLOR_BUFFER_BIT );

    glColor3f( 1.0, 1.0, 1.0 );
    glBegin( GL_TRIANGLES );
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 300.0, 0.0);
    glVertex3f(300.0, 0.0, 0.0);
    glEnd();

    glColor3f( 0.0, 1.0, 0.0 );
    glBegin( GL_TRIANGLES );
    glVertex3f(0.0, 300.0, 0.0);
    glVertex3f(300.0, 300.0, 0.0);
    glVertex3f(300.0, 0.0, 0.0);
    glEnd();

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

    glFlush();
    m_drawing_area.display();

    if( !m_drawing_area.renderWindow.setActive( false ) )
        return;

    on_action_draw_event2();
}
//------------------------------------------------------------------------------
bool Viewer_frame::on_action_draw_event( const ::Cairo::RefPtr<Cairo::Context>
  &cr )
{
    return on_action_draw_event2();
}
//------------------------------------------------------------------------------
bool Viewer_frame::on_action_draw_event2()
{
    draw_triangle();
    //draw_frame();
    return true;
}

