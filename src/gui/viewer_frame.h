#ifndef VIEWER_FRAME_H
#define VIEWER_FRAME_H

#include <gtkmm.h>
#include <gtkmm/socket.h>
#include <yuv/TYUV_file.h>
#include <gui/scroll_adapter.h>

class Viewer_frame : public Gtk::Window {

public:
  Viewer_frame();
  virtual ~Viewer_frame();

private:
  void on_action_file_quit();
  void on_action_show_size();
  bool on_action_configure_event( GdkEventConfigure *event );
  bool on_action_draw_event( const ::Cairo::RefPtr< ::Cairo::Context>& cr );
  void draw_triangle();

  Glib::RefPtr<Gtk::ActionGroup> m_action_group;
  Glib::RefPtr<Gtk::UIManager> m_ui_manager;
  Gtk::Box m_box;
  Gtk::DrawingArea m_drawing_area;

  Scroll_adapter m_scroll_adapter;
  Yuv_file m_yuv_file;
};

#endif // VIEWER_FRAME_H
