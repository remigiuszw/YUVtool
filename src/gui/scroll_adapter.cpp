#include <gui/scroll_adapter.h>

#include <iostream>

namespace YUV_tool {

Scroll_adapter::Scroll_adapter() :
    m_x_adjustment(Gtk::Adjustment::create(0, 0, 1, 1, 10, 1)),
    m_y_adjustment(Gtk::Adjustment::create(0, 0, 1, 1, 10, 1)),
    m_x_scrollbar(m_x_adjustment, Gtk::ORIENTATION_HORIZONTAL),
    m_y_scrollbar(m_y_adjustment, Gtk::ORIENTATION_VERTICAL),
    m_internal_size(1, 1)
{
    m_x_adjustment->signal_changed().connect(
            sigc::mem_fun(*this, &Scroll_adapter::on_scroll));
    m_x_adjustment->signal_value_changed().connect(
            sigc::mem_fun(*this, &Scroll_adapter::on_scroll));
    m_y_adjustment->signal_changed().connect(
            sigc::mem_fun(*this, &Scroll_adapter::on_scroll));
    m_y_adjustment->signal_value_changed().connect(
            sigc::mem_fun(*this, &Scroll_adapter::on_scroll));
    signal_size_allocate().connect(
            sigc::mem_fun(*this, &Scroll_adapter::on_signal_size_allocate));

    attach(m_x_scrollbar, 0, 1, 1, 1);
    attach(m_y_scrollbar, 1, 0, 1, 1);
}
//------------------------------------------------------------------------------
void Scroll_adapter::set_internal_size(const Vector<Unit::pixel> &size)
{
    m_internal_size = size;
    update_allocation();
}
//------------------------------------------------------------------------------
const Vector<Unit::pixel> &Scroll_adapter::get_internal_size() const
{
    return m_internal_size;
}
//------------------------------------------------------------------------------
Gdk::Rectangle Scroll_adapter::get_visible_area()
{
    Gtk::Allocation result;
    result.set_x(m_x_adjustment->get_value());
    result.set_y(m_y_adjustment->get_value());
    result.set_width(
            std::min<int>(
                m_internal_size.x(),
                m_x_adjustment->get_page_size()));
    result.set_height(
            std::min<int>(
                m_internal_size.y(),
                m_y_adjustment->get_page_size()));
    return result;
}
//------------------------------------------------------------------------------
sigc::signal<void> &Scroll_adapter::signal_post_scroll()
{
    return m_signal_post_scroll;
}
//------------------------------------------------------------------------------
void Scroll_adapter::on_scroll()
{
    signal_post_scroll()();
}
//------------------------------------------------------------------------------
void Scroll_adapter::on_signal_size_allocate(Gtk::Allocation &allocation)
{
    update_allocation();
}
//------------------------------------------------------------------------------
void Scroll_adapter::update_allocation()
{
    Gtk::Widget *child = get_child_at(0, 0);
    if(child)
    {
        m_x_adjustment->freeze_notify();
        m_y_adjustment->freeze_notify();
        const Gtk::Allocation inner_allocation = child->get_allocation();

        if(m_internal_size.x() > inner_allocation.get_width())
        {
            m_x_adjustment->set_page_size(inner_allocation.get_width());
            m_x_adjustment->set_upper(m_internal_size.x());

        }
        else
        {
            m_x_adjustment->set_page_size(inner_allocation.get_width());
            m_x_adjustment->set_upper(inner_allocation.get_width());
        }

        if(m_internal_size.y() > inner_allocation.get_height())
        {
            m_y_adjustment->set_page_size(inner_allocation.get_height());
            m_y_adjustment->set_upper(m_internal_size.y());

        }
        else
        {
            m_y_adjustment->set_page_size(inner_allocation.get_height());
            m_y_adjustment->set_upper(inner_allocation.get_height());
        }

        m_x_adjustment->thaw_notify();
        m_y_adjustment->thaw_notify();
    }
}

} /* YUV_tool */
