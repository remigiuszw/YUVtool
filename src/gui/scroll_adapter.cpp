#include <gui/scroll_adapter.h>

#include <gtkmm/adjustment.h>
#include <iostream>

namespace YUV_tool {

Scroll_adapter::Scroll_adapter()
{
    get_hadjustment()->signal_changed().connect(
            sigc::mem_fun(*this, &Scroll_adapter::on_scroll));
    get_hadjustment()->signal_value_changed().connect(
            sigc::mem_fun(*this, &Scroll_adapter::on_scroll));
    get_vadjustment()->signal_changed().connect(
            sigc::mem_fun(*this, &Scroll_adapter::on_scroll));
    get_vadjustment()->signal_value_changed().connect(
            sigc::mem_fun(*this, &Scroll_adapter::on_scroll));

    Gtk::ScrolledWindow::add(m_fixed);
}
//------------------------------------------------------------------------------
void Scroll_adapter::add(Gtk::Widget &widget)
{
    m_fixed.put(widget, 0, 0);
}
//------------------------------------------------------------------------------
void Scroll_adapter::remove()
{
    if(!m_fixed.get_children().empty())
        m_fixed.remove(*m_fixed.get_children().front());
    else
        std::cerr << "trying to remove child from scroll adapter, when none "
                "present\n";
}
//------------------------------------------------------------------------------
void Scroll_adapter::remove( Gtk::Widget &widget )
{
    m_fixed.remove( widget );
}
//------------------------------------------------------------------------------
void Scroll_adapter::set_internal_size( int width, int height )
{
    m_fixed.set_size_request( width, height );
}
//------------------------------------------------------------------------------
void Scroll_adapter::get_internal_size( int &width, int &height ) const
{
    m_fixed.get_size_request( width, height );
}
//------------------------------------------------------------------------------
Gdk::Rectangle Scroll_adapter::get_visible_area()
{
    int max_width, max_height;
    m_fixed.get_size_request( max_width, max_height );
    Gtk::Allocation result;
    result.set_x( get_hadjustment()->get_value() );
    result.set_y( get_vadjustment()->get_value() );
    result.set_width( std::min( max_width,
        static_cast<int>( get_hadjustment()->get_page_size() ) ) );
    result.set_height( std::min( max_height,
        static_cast<int>( get_vadjustment()->get_page_size() ) ) );
    return result;
}
//------------------------------------------------------------------------------
void Scroll_adapter::on_scroll()
{
    if(!m_fixed.get_children().empty())
    {
        Gtk::Widget &adapted = *m_fixed.get_children().front();
        const Gdk::Rectangle visible_area = get_visible_area();
        m_fixed.move(adapted, visible_area.get_x(), visible_area.get_y());
        adapted.set_size_request(
                visible_area.get_width(),
                visible_area.get_height());
    }
}
//------------------------------------------------------------------------------
void Scroll_adapter::on_size_allocate(Gtk::Allocation &allocation)
{
    Gtk::ScrolledWindow::on_size_allocate(allocation);
    on_scroll();
}

} /* YUV_tool */
