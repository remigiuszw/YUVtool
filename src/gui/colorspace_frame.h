/*
 * Copyright 2015, 2019 Dominik Wójt
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
#ifndef COLORSPACE_FRAME
#define COLORSPACE_FRAME

#include <yuv/Pixel_format.h>

#include <gtkmm/box.h>
#include <gtkmm/combobox.h>
#include <gtkmm/frame.h>
#include <gtkmm/label.h>
#include <gtkmm/liststore.h>
#include <gtkmm/notebook.h>
#include <gtkmm/spinbutton.h>

namespace YUV_tool {

class Colorspace_frame : public Gtk::Frame
{
private:
    struct Color_group
    {
        Gtk::Box m_box;
        Gtk::Label m_label;
        Gtk::SpinButton m_entry;

        Color_group(const std::string &name);
    };

    struct Component_configurator
    {
        Component_configurator();

        Gtk::Frame m_frame;
        Gtk::Box m_box;

        std::array<Color_group, Rgba_component_count> m_colors;

        Gtk::Box m_valid_range_box;
        Gtk::Label m_valid_range_label;
        Gtk::SpinButton m_valid_range_low_entry;
        Gtk::SpinButton m_valid_range_high_entry;

        Gtk::Box m_coded_range_box;
        Gtk::Label m_coded_range_label;
        Gtk::SpinButton m_coded_range_low_entry;
        Gtk::SpinButton m_coded_range_high_entry;
    };

    struct Color_space_column_record : Gtk::TreeModelColumnRecord
    {
        Gtk::TreeModelColumn<Glib::ustring> label;
        Gtk::TreeModelColumn<const Color_space *> pointer;

        Color_space_column_record();
    };

    Gtk::Box m_box;

    Gtk::Box m_predefined_box;
    Gtk::Label m_predefined_label;
    Gtk::ComboBox m_predefined_entry;

    Gtk::Box m_component_count_box;
    Gtk::Label m_component_count_label;
    Gtk::SpinButton m_component_count_entry;

    Gtk::Notebook m_component_box;
    std::array<Component_configurator, Rgba_component_count> m_components;

    Glib::RefPtr<Gtk::ListStore> m_predefined_list_store;
    const Color_space_column_record m_predefined_column_record;

    sigc::signal<void(const Color_space &)> m_color_space_changed_signal;

    bool m_update_in_progress{false};

public:
    Colorspace_frame();

    Color_space get_color_space() const;
    void set_color_space(const Color_space &color_space);

    sigc::signal<void(const Color_space &)> &color_space_changed_signal();

private:
    void update();
    void update_components(const Color_space &color_space);
};

}

#endif /* COLORSPACE_FRAME */
