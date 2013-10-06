#include "viewer_frame.h"
#include <iostream>
#include <cstdlib>
#include <gtk/gtkgl.h>
#include <GL/gl.h>

namespace {
void set_gl_capability( GtkWidget *widget ) {
  GdkGLConfigMode config_mode = static_cast<GdkGLConfigMode>(
    GDK_GL_MODE_RGB | GDK_GL_MODE_DEPTH | GDK_GL_MODE_DOUBLE );
  GdkGLConfig *glconfig = gdk_gl_config_new_by_mode ( config_mode );
  if(glconfig == NULL) {
    g_print ("\n*** Cannot find the double-buffered visual.\n");
    g_print ("\n*** Trying single-buffered visual.\n");

    /* Try single-buffered visual */
    config_mode = static_cast<GdkGLConfigMode>( GDK_GL_MODE_RGB |
      GDK_GL_MODE_DEPTH );
    glconfig = gdk_gl_config_new_by_mode( config_mode );
    if(glconfig == NULL) {
      g_print ("*** No appropriate OpenGL-capable visual found.\n");
      exit(1);
    }
  }
  /* Set OpenGL-capability to the widget */
  gtk_widget_set_gl_capability( widget, glconfig, NULL, TRUE,
    GDK_GL_RGBA_TYPE );
}
}
//------------------------------------------------------------------------------
Viewer_frame::Viewer_frame() :
  m_box( Gtk::ORIENTATION_VERTICAL )
{
  m_action_group = Gtk::ActionGroup::create();
  m_action_group->add( Gtk::Action::create( "menu_file", "_File" ) );
  m_action_group->add( Gtk::Action::create( "quit", Gtk::Stock::QUIT ),
    sigc::mem_fun(*this, &Viewer_frame::on_action_file_quit) );
  m_action_group->add( Gtk::Action::create( "show_size", Gtk::Stock::ABOUT,
    "Show size of the drowing area"),
    sigc::mem_fun(*this, &Viewer_frame::on_action_show_size) );

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

  Gtk::Widget *menu_bar = m_ui_manager->get_widget("/menu_bar");
  m_box.pack_start( *menu_bar, Gtk::PACK_SHRINK );

  Gtk::Widget *tool_bar = m_ui_manager->get_widget("/tool_bar");
  m_box.pack_start( *tool_bar, Gtk::PACK_SHRINK );

  set_gl_capability( &m_drawing_area.gobj()->widget );
  m_drawing_area.signal_configure_event().connect(
    sigc::mem_fun( *this, &Viewer_frame::on_action_configure_event ) );
  m_drawing_area.signal_draw().connect(
    sigc::mem_fun( *this, &Viewer_frame::on_action_draw_event ) );
  m_box.pack_start( m_drawing_area, Gtk::PACK_EXPAND_WIDGET );

  add(m_box);

  show_all();
}
//------------------------------------------------------------------------------
Viewer_frame::~Viewer_frame() {
}
//------------------------------------------------------------------------------
void Viewer_frame::on_action_file_quit() {
  hide();
}
//------------------------------------------------------------------------------
void Viewer_frame::on_action_show_size() {
  Gtk::MessageDialog dialog( *this, "Size of the drawing area." );

  std::stringstream ss;
  ss << "The size of the drawing area is " <<
    m_drawing_area.get_width() << 'x' <<
    m_drawing_area.get_height();
  dialog.set_secondary_text( ss.str() );

  dialog.run();
}
//------------------------------------------------------------------------------
bool Viewer_frame::on_action_configure_event( GdkEventConfigure* event ) {
  std::cout << "The size of the drawing area is " <<
    m_drawing_area.get_width() << 'x' <<
    m_drawing_area.get_height() << '\n';

  GtkWidget *widget = &m_drawing_area.gobj()->widget;
  GdkGLContext *glcontext = gtk_widget_get_gl_context( widget );
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable( widget );
  const int width = m_drawing_area.get_width();
  const int height = m_drawing_area.get_height();

  if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext))
    g_assert_not_reached ();

  glViewport( 0, 0, width, height );

  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glOrtho( 0, 1, 0, 1, -1, 1 );

  glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
  glDisable( GL_DEPTH_TEST );

  gdk_gl_drawable_gl_end (gldrawable);

  return true;
}
//------------------------------------------------------------------------------
void Viewer_frame::draw_triangle() {
  GtkWidget *widget = &m_drawing_area.gobj()->widget;
  GdkGLContext *glcontext = gtk_widget_get_gl_context( widget );
  GdkGLDrawable *gldrawable = gtk_widget_get_gl_drawable( widget );

  if (!gdk_gl_drawable_gl_begin (gldrawable, glcontext))
    g_assert_not_reached ();

  glClearColor( 0.0, 0.2, 0.0, 0.0 );
  glClear( GL_COLOR_BUFFER_BIT );
  glColor3f( 1.0, 1.0, 1.0 );
  glBegin( GL_TRIANGLES );
    glVertex3f( 0.0, 0.0, 0.0 );
    glVertex3f( 1.0, 0.0, 0.0 );
    glVertex3f( 0.5, 1.0, 0.0 );
  glEnd();

  if( gdk_gl_drawable_is_double_buffered( gldrawable ) )
    gdk_gl_drawable_swap_buffers( gldrawable );
  else
    glFlush();

  gdk_gl_drawable_gl_end( gldrawable );
}
//------------------------------------------------------------------------------
bool Viewer_frame::on_action_draw_event(const::Cairo::RefPtr<Cairo::Context>
  &cr ) {
  draw_triangle();
  return true;
}

