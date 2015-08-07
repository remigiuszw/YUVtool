#ifndef SCROLL_ADAPTER_H
#define SCROLL_ADAPTER_H

#include <yuv/coordinates.h>

#include <gtkmm/grid.h>
#include <gtkmm/bin.h>
#include <gtkmm/scrollbar.h>
#include <gtkmm/adjustment.h>

namespace YUV_tool {

class Scroll_adapter : public Gtk::Grid
{
private:
    Glib::RefPtr<Gtk::Adjustment> m_x_adjustment;
    Glib::RefPtr<Gtk::Adjustment> m_y_adjustment;
    Gtk::Scrollbar m_x_scrollbar;
    Gtk::Scrollbar m_y_scrollbar;
    Vector<Unit::pixel> m_internal_size;
    Gtk::Widget *m_widget;
    sigc::signal<void> m_signal_post_scroll;

public:
    Scroll_adapter();
    void set_internal_size(const Vector<Unit::pixel> &size);
    const Vector<Unit::pixel> &get_internal_size() const;
    Gdk::Rectangle get_visible_area();
    sigc::signal<void> &signal_post_scroll();

private:
    void on_scroll();
    void on_signal_size_allocate(Gtk::Allocation &allocation);
    void update_allocation();
};

} /* namespace YUV_tool */

#endif // SCROLL_ADAPTER_H
