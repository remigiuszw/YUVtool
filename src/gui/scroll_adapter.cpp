/* 
 * Copyright 2015 Dominik Wójt
 * 
 * This file is part of YUVtool.
 * 
 * YUVtool is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * YUVtool is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with YUVtool.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
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

    signal_size_allocate().connect_notify(
            std::bind(
                sigc::mem_fun(*this, &Scroll_adapter::update_allocation)));
    m_drawing_area.signal_post_size_allocate().connect(
            sigc::mem_fun(*this, &Scroll_adapter::update_allocation));
    m_drawing_area.signal_draw().connect(
            sigc::mem_fun(*this, &Scroll_adapter::kick_signal_update_drawing));

    m_drawing_area.set_hexpand();
    m_drawing_area.set_vexpand();
    attach(m_drawing_area, 0, 0, 1, 1);
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
    result.set_width(m_x_adjustment->get_page_size());
    result.set_height(m_y_adjustment->get_page_size());
    return result;
}
//------------------------------------------------------------------------------
sigc::signal<void> &Scroll_adapter::signal_update_viewport()
{
    return m_signal_update_viewport;
}
//------------------------------------------------------------------------------
sigc::signal<void> &Scroll_adapter::signal_update_drawing()
{
    return m_signal_update_drawing;
}
//------------------------------------------------------------------------------
bool Scroll_adapter::set_active(bool active)
{
    return m_drawing_area.set_active(active);
}
//------------------------------------------------------------------------------
void Scroll_adapter::display()
{
    m_drawing_area.display();
}
//------------------------------------------------------------------------------
void Scroll_adapter::on_scroll()
{
    signal_update_viewport()();
    signal_update_drawing()();
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
            m_x_adjustment->set_value(0);
        }

        if(m_internal_size.y() > inner_allocation.get_height())
        {
            m_y_adjustment->set_page_size(inner_allocation.get_height());
            m_y_adjustment->set_upper(m_internal_size.y());
            m_y_adjustment->set_value(0);
        }
        else
        {
            m_y_adjustment->set_page_size(inner_allocation.get_height());
            m_y_adjustment->set_upper(inner_allocation.get_height());
        }

        m_x_adjustment->thaw_notify();
        m_y_adjustment->thaw_notify();

//        std::cerr << "Scroll_adapter::update_allocation()" << std::endl;
        on_scroll();
    }
}
//------------------------------------------------------------------------------
bool Scroll_adapter::kick_signal_update_drawing(
        const Cairo::RefPtr<Cairo::Context>)
{
    m_signal_update_drawing();
    return true;
}

} /* YUV_tool */
