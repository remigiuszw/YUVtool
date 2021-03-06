#ifndef VIEWER_FRAME_H
#define VIEWER_FRAME_H

#include <gui/scroll_adapter.h>
#include <gui/drawer_gl.h>
#include <yuv/Yuv_file.h>

#include <gtkmm/window.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>

namespace YUV_tool {

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
    void on_action_help_info();
    void on_action_help_about();
    void on_action_size_allocation();
    void on_action_draw_event();
    void draw_triangle();
    void draw_frame();

    Glib::RefPtr<Gtk::ActionGroup> m_action_group;
    Glib::RefPtr<Gtk::UIManager> m_ui_manager;
    Gtk::Box m_box;
    Scroll_adapter m_scroll_adapter;
    Drawer_gl m_drawer_gl;

    Yuv_file m_yuv_file;
};

} /* namespace YUV_tool */

#endif // VIEWER_FRAME_H
