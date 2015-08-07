/* copied from https://github.com/LaurentGomila/SFML/wiki/Source%3A-GTK-SFMLWidget#wiki-sfmlwidgeth
 * public domain */

#ifndef SFMLWIDGET_H_INCLUDED
#define SFMLWIDGET_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <gtkmm/widget.h>

namespace YUV_tool {

class SFMLWidget : public Gtk::Widget
{
protected:
    virtual void on_size_allocate(Gtk::Allocation& allocation);
    void on_realize() override;
    void on_realize_internal();
    void on_unrealize() override;
    void on_unrealize_internal();

    Glib::RefPtr<Gdk::Window> m_refGdkWindow;
    sigc::signal<void, Gtk::Allocation &> m_signal_post_size_allocate;
public:
    sf::RenderWindow renderWindow;

    SFMLWidget(sf::VideoMode mode, int size_request=-1);
    virtual ~SFMLWidget();

    sigc::signal<void, Gtk::Allocation &> &signal_post_size_allocate();
    void invalidate();
    void display();
};

} /* YUV_tool */

#endif
