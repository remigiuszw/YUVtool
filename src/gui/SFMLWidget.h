/* copied from https://github.com/LaurentGomila/SFML/wiki/Source%3A-GTK-SFML_widget#wiki-SFML_widgeth
 * public domain */

#ifndef SFML_widget_H_INCLUDED
#define SFML_widget_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <gtkmm/widget.h>
#include <glibmm/main.h>

namespace YUV_tool {

class SFML_widget : public Gtk::Widget
{
private:
    Glib::RefPtr<Gdk::Window> m_ref_gdk_window;
    sf::RenderWindow m_render_window;
    sigc::signal<void> m_signal_post_size_allocate;
    Glib::RefPtr<Glib::TimeoutSource> m_timeout_source;

protected:
    void on_size_allocate(Gtk::Allocation& allocation) override;
    void on_realize() override;
    void on_realize_internal();
    void on_unrealize() override;
    void on_unrealize_internal();
    bool on_timeout();

public:
    SFML_widget();
    ~SFML_widget() override;

    sigc::signal<void> &signal_post_size_allocate();
    bool set_active(bool active);
    void display();
};

} /* YUV_tool */

#endif
