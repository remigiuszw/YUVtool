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
#ifndef SCROLL_ADAPTER_H
#define SCROLL_ADAPTER_H

#include <yuv/Coordinates.h>

#include <gtkmm/grid.h>
#include <gtkmm/scrollbar.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/glarea.h>

namespace YUV_tool {

class Scroll_adapter : public Gtk::Grid
{
private:
    Glib::RefPtr<Gtk::Adjustment> m_x_adjustment;
    Glib::RefPtr<Gtk::Adjustment> m_y_adjustment;
    Gtk::Scrollbar m_x_scrollbar;
    Gtk::Scrollbar m_y_scrollbar;
    Vector<Unit::pixel> m_internal_size;
    Gtk::GLArea m_drawing_area;

public:
    Scroll_adapter();
    void set_internal_size(const Vector<Unit::pixel> &size);
    const Vector<Unit::pixel> &get_internal_size() const;
    /* might return area larger than internal size */
    Gdk::Rectangle get_visible_area();
    Gtk::GLArea &get_drawing_area();

private:
    void update_allocation(const Gtk::Allocation &allocation);
    void on_scroll();
};

} /* namespace YUV_tool */

#endif // SCROLL_ADAPTER_H
