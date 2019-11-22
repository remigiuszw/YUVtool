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
#ifndef FORMAT_CHOOSER_DIALOG_H
#define FORMAT_CHOOSER_DIALOG_H

#include <colorspace_frame.h>

#include <yuv/Pixel_format.h>

#include <gtkmm/combobox.h>
#include <gtkmm/dialog.h>
#include <gtkmm/frame.h>
#include <gtkmm/grid.h>
#include <gtkmm/liststore.h>
#include <gtkmm/spinbutton.h>

namespace YUV_tool {

class Format_chooser_dialog : public Gtk::Dialog
{
public:
    Format_chooser_dialog(
            Gtk::Window &parent,
            const Pixel_format &default_pixel_format);
    virtual ~Format_chooser_dialog();
    const Pixel_format &get_pixel_format() const;

private:
    struct Import_box : Gtk::Box
    {
        Import_box();

        Gtk::Label m_label;
        Gtk::ComboBox m_choice;
    };

    struct Entry_configurator : Gtk::Box
    {
        Entry_configurator();

        Gtk::Label m_entry_label;
        Gtk::SpinButton m_bit_count_entry;
        Gtk::Label m_bits_label;
    };

    struct Row_in_plane_configurator : Gtk::Box
    {
        Row_in_plane_configurator();
        void set_entries_count(Index n);

        Gtk::Box m_entry_count_box;
        Gtk::Label m_entry_count_label;
        Gtk::SpinButton m_entry_count_entry;

        std::vector<std::unique_ptr<Entry_configurator> > m_entries;
    };

    struct Plane_configurator : Gtk::Frame
    {
        Plane_configurator();
        void set_rows_count(Index n);

        Gtk::Box m_box;

        Gtk::Box m_row_count_box;
        Gtk::Label m_row_count_label;
        Gtk::SpinButton m_row_count_entry;

        std::vector<std::unique_ptr<Row_in_plane_configurator> > m_rows;
    };

    struct Plane_frame : Gtk::Frame
    {
        Plane_frame();
        void set_planes_count(Index n);

        Gtk::Box m_box;

        Gtk::Box m_planes_count_box;
        Gtk::Label m_planes_count_label;
        Gtk::SpinButton m_planes_count_entry;

        Gtk::Box m_planes_box;
        std::vector<std::unique_ptr<Plane_configurator> > m_planes;
    };

    struct Sample_configurator : Gtk::Box
    {
        Sample_configurator();

        Gtk::Label m_sample_label;
        Gtk::Label m_plane_label;
        Gtk::SpinButton m_plane_entry;
        Gtk::Label m_row_label;
        Gtk::SpinButton m_row_entry;
        Gtk::Label m_index_label;
        Gtk::SpinButton m_index_entry;
    };

    struct Pixel_configurator : Gtk::Frame
    {
        Pixel_configurator();
        void set_samples_count(Index n);

        Gtk::Box m_box;
        std::vector<std::unique_ptr<Sample_configurator> > m_samples;
    };

    struct Macropixel_frame : Gtk::Frame
    {
        Macropixel_frame();
        void reshape();

        Gtk::Box m_box;

        Gtk::Box m_parameters_box;
        Gtk::Label m_rows_label;
        Gtk::SpinButton m_rows_entry;
        Gtk::Label m_columns_label;
        Gtk::SpinButton m_columns_entry;

        Gtk::Grid m_pixel_grid;
        std::vector<std::unique_ptr<Pixel_configurator> > m_pixels;
    };

    struct Pixel_format_column_record : Gtk::TreeModel::ColumnRecord
    {
        Gtk::TreeModelColumn<Glib::ustring> m_label;
        Gtk::TreeModelColumn<const Pixel_format *> m_pointer;

        Pixel_format_column_record()
        {
            add(m_label);
            add(m_pointer);
        }
    };

    void update_format();
    void on_predefined_format();
    void on_import_format();
    void on_predefined_color_space();
    void on_components_count();
    void on_component(Rgba_component c);

    Gtk::ComboBox m_predefined_choice;
    Import_box m_import_box;
    Plane_frame m_plane_frame;
    Colorspace_frame m_colorspace_frame;
    Macropixel_frame m_macropixel_frame;

    Glib::RefPtr<Gtk::ListStore> m_predefined_list_store;
    Glib::RefPtr<Gtk::ListStore> m_import_list_store;
    Pixel_format_column_record m_pixel_format_column_record;

    Pixel_format m_pixel_format;
};

}

#endif /* FORMAT_CHOOSER_DIALOG_H */
