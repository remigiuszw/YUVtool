/*
 * Copyright 2019 Dominik Wójt
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
#include <resolution_and_format_dialog.h>

namespace YUV_tool {
/*----------------------------------------------------------------------------*/
Resolution_and_format_dialog::Resolution_and_format_dialog(
    Gtk::Window& parent) :
    Gtk::Dialog("Choose resolution and pixel format", parent, true)
{
    add_button("OK", Gtk::RESPONSE_OK);
    add_button("Cancel", Gtk::RESPONSE_CANCEL);
    /* TODO: add preview button */

    get_content_area()->add(m_box);

    m_box.pack_start(m_format_widget);
    m_box.pack_start(m_resolution_widget);

    m_format_widget.signal_pixel_format_changed().connect(
        [this]() { m_signal_pixel_format_changed(); });
    m_resolution_widget.signal_resolution_changed().connect(
        [this]() { m_signal_resolution_changed(); });

    show_all();
}
/*----------------------------------------------------------------------------*/
Pixel_format Resolution_and_format_dialog::get_pixel_format() const
{
    return m_format_widget.get_pixel_format();
}
/*----------------------------------------------------------------------------*/
void Resolution_and_format_dialog::set_pixel_format(
    const Pixel_format& pixel_format)
{
    m_format_widget.set_pixel_format(pixel_format);
}
/*----------------------------------------------------------------------------*/
Vector<Unit::pixel> Resolution_and_format_dialog::get_resolution() const
{
    return m_resolution_widget.get_resolution();
}
/*----------------------------------------------------------------------------*/
void Resolution_and_format_dialog::set_resolution(
    const Vector<Unit::pixel>& resolution)
{
    m_resolution_widget.set_resolution(resolution);
}
/*----------------------------------------------------------------------------*/
} /* namespace YUV_tool */
