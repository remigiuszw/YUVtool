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
#include <gui/format_chooser_dialog.h>

namespace YUV_tool {
/*----------------------------------------------------------------------------*/
Format_chooser_dialog::Format_chooser_dialog(
        Gtk::Window &parent,
        const Pixel_format &default_pixel_format) :
    Gtk::Dialog("Choose pixel format", parent, true),
    m_pixel_format(default_pixel_format)
{
    add_button("OK", Gtk::RESPONSE_OK);
    add_button("Cancel", Gtk::RESPONSE_CANCEL);
    /* TODO: add preview button */

    Gtk::Box &content_area = *get_content_area();
    content_area.pack_start(m_predefined_choice);
    content_area.pack_start(m_import_box);
    content_area.pack_start(m_plane_frame);
    content_area.pack_start(m_colorspace_frame);
    content_area.pack_start(m_macropixel_frame);

    /* predefined choice */
    {
        m_predefined_list_store =
                Gtk::ListStore::create(m_pixel_format_column_record);
        Gtk::ListStore::iterator iter;
        iter = m_predefined_list_store->append();
        (*iter)[m_pixel_format_column_record.m_label] = "yuv420p";
        (*iter)[m_pixel_format_column_record.m_pointer] = &yuv_420p_8bit;
        iter = m_predefined_list_store->append();
        (*iter)[m_pixel_format_column_record.m_label] = "rgb32bpp";
        (*iter)[m_pixel_format_column_record.m_pointer] = &rgb_32bpp;
        iter = m_predefined_list_store->append();
        (*iter)[m_pixel_format_column_record.m_label] = "custom";
        (*iter)[m_pixel_format_column_record.m_pointer] = nullptr;
        m_predefined_choice.set_model(m_predefined_list_store);
        m_predefined_choice.pack_start(m_pixel_format_column_record.m_label);
        m_predefined_choice.set_active(iter);
    }

    /* import choice */
    {
        m_import_list_store =
                Gtk::ListStore::create(m_pixel_format_column_record);
        Gtk::ListStore::iterator iter;
        iter = m_import_list_store->append();
        (*iter)[m_pixel_format_column_record.m_label] = "yuv420p";
        (*iter)[m_pixel_format_column_record.m_pointer] = &yuv_420p_8bit;
        iter = m_import_list_store->append();
        (*iter)[m_pixel_format_column_record.m_label] = "rgb32bpp";
        (*iter)[m_pixel_format_column_record.m_pointer] = &rgb_32bpp;
        iter = m_import_list_store->append();
        (*iter)[m_pixel_format_column_record.m_label] = "...";
        (*iter)[m_pixel_format_column_record.m_pointer] = nullptr;
        m_import_box.m_import_choice.set_model(m_import_list_store);
        m_import_box.m_import_choice.pack_start(
                    m_pixel_format_column_record.m_label);
        m_import_box.m_import_choice.set_active(iter);
    }

    m_predefined_choice.signal_changed().connect(
                sigc::mem_fun(*this, &Format_chooser_dialog::update_format));
    m_import_box.m_import_button.signal_clicked().connect(
                sigc::mem_fun(*this, &Format_chooser_dialog::import_format));

    update_format();

    show_all();
}
/*----------------------------------------------------------------------------*/
Format_chooser_dialog::~Format_chooser_dialog()
{ }
/*----------------------------------------------------------------------------*/
const Pixel_format &Format_chooser_dialog::get_pixel_format() const
{
    return m_pixel_format;
}
/*----------------------------------------------------------------------------*/
Format_chooser_dialog::Import_box::Import_box() :
    Gtk::Box(Gtk::ORIENTATION_HORIZONTAL),
    m_import_button("Import")
{
    pack_start(m_import_choice);
    pack_start(m_import_button);
}
/*----------------------------------------------------------------------------*/
Format_chooser_dialog::Entry_configurator::Entry_configurator() :
    Gtk::Box(Gtk::ORIENTATION_HORIZONTAL),
    m_entry_label("entry:"),
    m_bits_label("bits")
{
    pack_start(m_entry_label);
    pack_start(m_bit_count_entry);
    pack_start(m_bits_label);
}
/*----------------------------------------------------------------------------*/
Format_chooser_dialog::Row_in_plane_configurator::Row_in_plane_configurator() :
    Gtk::Box(Gtk::ORIENTATION_VERTICAL),
    m_entry_count_box(Gtk::ORIENTATION_HORIZONTAL),
    m_row_label("entries count:")
{
    pack_start(m_entry_count_box);
    pack_start(m_row_label);
    pack_start(m_entry_count_entry);
}
/*----------------------------------------------------------------------------*/
Format_chooser_dialog::Plane_configurator::Plane_configurator() :
    m_box(Gtk::ORIENTATION_VERTICAL),
    m_row_count_label("rows count:")
{
    add(m_box);
    m_box.pack_start(m_row_count_box);
    m_box.pack_start(m_row_count_label);
    m_box.pack_start(m_row_count_entry);
}
/*----------------------------------------------------------------------------*/
Format_chooser_dialog::Plane_frame::Plane_frame() :
    m_box(Gtk::ORIENTATION_HORIZONTAL)
{ }
/*----------------------------------------------------------------------------*/
Format_chooser_dialog::Component_configurator::Component_configurator() :
    m_box(Gtk::ORIENTATION_VERTICAL),
    m_r_box(Gtk::ORIENTATION_HORIZONTAL),
    m_r_label("R"),
    m_g_box(Gtk::ORIENTATION_HORIZONTAL),
    m_g_label("G"),
    m_b_box(Gtk::ORIENTATION_HORIZONTAL),
    m_b_label("B"),
    m_a_box(Gtk::ORIENTATION_HORIZONTAL),
    m_a_label("A")
{
    add(m_box);
    m_box.pack_start(m_r_box);
    m_r_box.pack_start(m_r_label);
    m_r_box.pack_start(m_r_entry);
    m_box.pack_start(m_g_box);
    m_g_box.pack_start(m_g_label);
    m_g_box.pack_start(m_g_entry);
    m_box.pack_start(m_b_box);
    m_b_box.pack_start(m_b_label);
    m_b_box.pack_start(m_b_entry);
    m_box.pack_start(m_a_box);
    m_a_box.pack_start(m_a_label);
    m_a_box.pack_start(m_a_entry);
}
/*----------------------------------------------------------------------------*/
Format_chooser_dialog::Colorspace_frame::Colorspace_frame() :
    m_box(Gtk::ORIENTATION_VERTICAL),
    m_predefined_colorspace_box(Gtk::ORIENTATION_HORIZONTAL),
    m_predefined_colorspace_label("colorspace:"),
    m_component_count_box(Gtk::ORIENTATION_HORIZONTAL),
    m_component_count_label("components_count:"),
    m_component_box(Gtk::ORIENTATION_HORIZONTAL)
{
    add(m_box);
    m_box.pack_start(m_predefined_colorspace_box);
    m_predefined_colorspace_box.pack_start(m_predefined_colorspace_label);
    m_predefined_colorspace_box.pack_start(m_predefined_colorspace_entry);
    m_box.pack_start(m_component_count_box);
    m_component_count_box.pack_start(m_component_count_label);
    m_component_count_box.pack_start(m_component_count_entry);
    m_box.pack_start(m_component_box);
}
/*----------------------------------------------------------------------------*/
Format_chooser_dialog::Sample_configurator::Sample_configurator() :
    Gtk::Box(Gtk::ORIENTATION_HORIZONTAL),
    m_sample_label("sample:"),
    m_plane_label("plane:"),
    m_row_label("row"),
    m_column_label("column")
{
    pack_start(m_sample_label);
    pack_start(m_plane_label);
    pack_start(m_plane_entry);
    pack_start(m_row_label);
    pack_start(m_row_entry);
    pack_start(m_column_label);
    pack_start(m_column_entry);
}
/*----------------------------------------------------------------------------*/
Format_chooser_dialog::Pixel_configurator::Pixel_configurator() :
    m_box(Gtk::ORIENTATION_VERTICAL)
{
    add(m_box);
}
/*----------------------------------------------------------------------------*/
Format_chooser_dialog::Macropixel_frame::Macropixel_frame() :
    m_box(Gtk::ORIENTATION_VERTICAL),
    m_parameters_box(Gtk::ORIENTATION_HORIZONTAL),
    m_rows_label("rows:"),
    m_columns_label("columns:")
{
    add(m_box);
    m_box.pack_start(m_parameters_box);
    m_parameters_box.pack_start(m_rows_label);
    m_parameters_box.pack_start(m_rows_entry);
    m_parameters_box.pack_start(m_columns_label);
    m_parameters_box.pack_start(m_columns_entry);
    m_box.pack_start(m_pixel_grid);
}
/*----------------------------------------------------------------------------*/
void Format_chooser_dialog::import_format()
{
    const Pixel_format *imported_pixel_format =
            (*m_import_box.m_import_choice.get_active())[
                m_pixel_format_column_record.m_pointer];

    if(imported_pixel_format != nullptr)
    {
        m_pixel_format = *imported_pixel_format;
        update_format();
    }
    auto options_end = m_import_list_store->children().end();
    options_end--;
    m_import_box.m_import_choice.set_active(options_end);
}
/*----------------------------------------------------------------------------*/
void Format_chooser_dialog::update_format()
{
    const Pixel_format *predefined_format =
            (*m_predefined_choice.get_active())[
                m_pixel_format_column_record.m_pointer];
    if(predefined_format != nullptr)
    {
        m_pixel_format = *predefined_format;
        m_import_box.hide();
        m_plane_frame.hide();
        m_colorspace_frame.hide();
        m_macropixel_frame.hide();
    }
    else
    {
        m_import_box.show_all();
        m_plane_frame.show_all();
        m_colorspace_frame.show_all();
        m_macropixel_frame.show_all();
    }
}
/*----------------------------------------------------------------------------*/
}
