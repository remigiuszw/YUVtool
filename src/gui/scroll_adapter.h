#ifndef SCROLL_ADAPTER_H
#define SCROLL_ADAPTER_H

#include <gtkmm/scrolledwindow.h>
#include <gtkmm/fixed.h>

namespace YUV_tool {

class Scroll_adapter : public Gtk::ScrolledWindow
{
public:
    Scroll_adapter();
    void add(Gtk::Widget &widget);
    void remove();
    void remove(Gtk::Widget &widget);
    void set_internal_size(int width, int height);
    void get_internal_size(int &width, int &height) const;
    Gdk::Rectangle get_visible_area();

private:
    void on_scroll();
    void on_size_allocate(Gtk::Allocation &allocation) override;

    Gtk::Fixed m_fixed;
};

} /* namespace YUV_tool */

#endif // SCROLL_ADAPTER_H
