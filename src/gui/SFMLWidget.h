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
    sigc::connection m_timeout_connection;
    Glib::RefPtr<Gdk::Window> m_ref_gdk_window;
    sigc::signal<void> m_signal_post_size_allocate;
    sf::RenderWindow m_render_window;
    Glib::RefPtr<Glib::TimeoutSource> m_timeout_source;

protected:
    virtual void on_size_allocate(Gtk::Allocation& allocation);
    void on_realize() override;
    void on_realize_internal();
    void on_unrealize() override;
    void on_unrealize_internal();
    bool on_timeout();

public:
    SFML_widget(sf::VideoMode mode, int size_request=-1);
    virtual ~SFML_widget();

    sigc::signal<void> &signal_post_size_allocate();
    sf::RenderWindow &render_window();
    void invalidate();
    void display();
};

} /* YUV_tool */

#endif
