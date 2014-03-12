#ifndef VIEWER_FRAME_H
#define VIEWER_FRAME_H

#include <gui/SFMLWidget.h>
#include <gtkmm.h>
#include <gtkmm/socket.h>
#include <yuv/TYUV_file.h>
#include <gui/scroll_adapter.h>
#include <gui/drawer_gl.h>

class Viewer_frame : public Gtk::Window
{
public:
    Viewer_frame();
    virtual ~Viewer_frame();

private:
    void on_action_file_quit();
    void on_action_show_size();
    void on_action_file_open();
    void on_action_file_close();
    bool on_action_configure_event( GdkEventConfigure *event );
    void on_action_size_allocation( Gtk::Allocation &allocation );
    bool on_action_draw_event( const ::Cairo::RefPtr< ::Cairo::Context>& cr );
    void draw_triangle();
    void draw_frame();

    Glib::RefPtr<Gtk::ActionGroup> m_action_group;
    Glib::RefPtr<Gtk::UIManager> m_ui_manager;
    Gtk::Box m_box;
    SFMLWidget m_drawing_area;
    Scroll_adapter m_scroll_adapter;
    Drawer_gl m_drawer_gl;

    Yuv_file m_yuv_file;
};

#endif // VIEWER_FRAME_H
