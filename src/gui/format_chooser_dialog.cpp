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
#include <format_chooser_dialog.h>

#define DEBUG 1

namespace YUV_tool {
/*----------------------------------------------------------------------------*/
namespace {
/*----------------------------------------------------------------------------*/
template<typename TWidget>
void set_children_count(
        Gtk::Box &box,
        std::vector<std::unique_ptr<TWidget>> &vector,
        const Index n)
{
    while (vector.size() < n)
    {
        vector.emplace_back(new TWidget);
        box.pack_start(*vector.back());
    }
    while (vector.size() > n)
    {
        box.remove(*vector.back());
        vector.pop_back();
    }
}
/*----------------------------------------------------------------------------*/
}
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
    content_area.pack_start(m_colorspace_frame);
    content_area.pack_start(m_plane_frame);
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
        m_import_box.m_choice.set_model(m_import_list_store);
        m_import_box.m_choice.pack_start(
                    m_pixel_format_column_record.m_label);
        m_import_box.m_choice.set_active(iter);
    }

    m_predefined_choice.signal_changed().connect(
        sigc::mem_fun(*this, &Format_chooser_dialog::on_predefined_format));
    m_import_box.m_choice.signal_changed().connect(
        sigc::mem_fun(*this, &Format_chooser_dialog::on_import_format));

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
    m_label("Import:")
{
    pack_start(m_label);
    pack_start(m_choice);
}
/*----------------------------------------------------------------------------*/
Format_chooser_dialog::Entry_configurator::Entry_configurator() :
    Gtk::Box(Gtk::ORIENTATION_HORIZONTAL),
    m_entry_label("entry:"),
    m_bit_count_entry(Gtk::Adjustment::create(0, 0, 100, 1)),
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
    m_entry_count_label("entries count:"),
    m_entry_count_entry(Gtk::Adjustment::create(0, 0, 100, 1))
{
    pack_start(m_entry_count_box);
    m_entry_count_box.pack_start(m_entry_count_label);
    m_entry_count_box.pack_start(m_entry_count_entry);
}
/*----------------------------------------------------------------------------*/
void Format_chooser_dialog::Row_in_plane_configurator::set_entries_count(
        const Index n)
{
    set_children_count(*this, m_entries, n);
}
/*----------------------------------------------------------------------------*/
Format_chooser_dialog::Plane_configurator::Plane_configurator() :
    m_box(Gtk::ORIENTATION_VERTICAL),
    m_row_count_label("rows count:"),
    m_row_count_entry(Gtk::Adjustment::create(0, 0, 100, 1))
{
    add(m_box);
    m_box.pack_start(m_row_count_box);
    m_row_count_box.pack_start(m_row_count_label);
    m_row_count_box.pack_start(m_row_count_entry);
}
/*----------------------------------------------------------------------------*/
void Format_chooser_dialog::Plane_configurator::set_rows_count(const Index n)
{
    set_children_count(m_box, m_rows, n);
}
/*----------------------------------------------------------------------------*/
Format_chooser_dialog::Plane_frame::Plane_frame() :
    m_box(Gtk::ORIENTATION_VERTICAL),
    m_planes_count_box(Gtk::ORIENTATION_HORIZONTAL),
    m_planes_count_label("planes count: "),
    m_planes_count_entry(Gtk::Adjustment::create(0, 0, 100, 1)),
    m_planes_box(Gtk::ORIENTATION_HORIZONTAL)
{
    set_label("Planes");
    set_shadow_type(Gtk::SHADOW_ETCHED_OUT);

    add(m_box);
    m_box.pack_start(m_planes_count_box);
    m_planes_count_box.pack_start(m_planes_count_label);
    m_planes_count_box.pack_start(m_planes_count_entry);
    m_box.pack_start(m_planes_box);
}
/*----------------------------------------------------------------------------*/
void Format_chooser_dialog::Plane_frame::set_planes_count(const Index n)
{
    set_children_count(m_planes_box, m_planes, n);
}

/*----------------------------------------------------------------------------*/
Format_chooser_dialog::Sample_configurator::Sample_configurator() :
    Gtk::Box(Gtk::ORIENTATION_HORIZONTAL),
    m_sample_label("sample:"),
    m_plane_label("plane:"),
    m_row_label("row"),
    m_index_label("index")
{
    pack_start(m_sample_label);
    pack_start(m_plane_label);
    pack_start(m_plane_entry);
    pack_start(m_row_label);
    pack_start(m_row_entry);
    pack_start(m_index_label);
    pack_start(m_index_entry);
}
/*----------------------------------------------------------------------------*/
Format_chooser_dialog::Pixel_configurator::Pixel_configurator() :
    m_box(Gtk::ORIENTATION_VERTICAL)
{
    add(m_box);
}
/*----------------------------------------------------------------------------*/
void Format_chooser_dialog::Pixel_configurator::set_samples_count(
        const Index n)
{
    set_children_count(m_box, m_samples, n);
}
/*----------------------------------------------------------------------------*/
Format_chooser_dialog::Macropixel_frame::Macropixel_frame() :
    m_box(Gtk::ORIENTATION_VERTICAL),
    m_parameters_box(Gtk::ORIENTATION_HORIZONTAL),
    m_rows_label("rows:"),
    m_rows_entry(Gtk::Adjustment::create(0, 0, 100, 1)),
    m_columns_label("columns:"),
    m_columns_entry(Gtk::Adjustment::create(0, 0, 100, 1))
{
    set_label("Macropixel");
    set_shadow_type(Gtk::SHADOW_ETCHED_OUT);

    add(m_box);
    m_box.pack_start(m_parameters_box);
    m_parameters_box.pack_start(m_rows_label);
    m_parameters_box.pack_start(m_rows_entry);
    m_parameters_box.pack_start(m_columns_label);
    m_parameters_box.pack_start(m_columns_entry);
    m_box.pack_start(m_pixel_grid);
}
/*----------------------------------------------------------------------------*/
void Format_chooser_dialog::Macropixel_frame::reshape()
{
    const Vector<Unit::pixel> mp_size(
                m_columns_entry.get_value_as_int(),
                m_rows_entry.get_value_as_int());
    const Index n = mp_size.x() * mp_size.y();

    for (auto &pixel : m_pixels)
        m_pixel_grid.remove(*pixel);

    while (m_pixels.size() < n)
    {
        m_pixels.emplace_back(new Pixel_configurator);
    }
    while (m_pixels.size() > n)
    {
        m_pixels.pop_back();
    }

    auto pixels_range =
            make_rectangle(
                Coordinates<Unit::pixel, Reference_point::macropixel>(0, 0),
                mp_size);
    for (auto xy : pixels_range)
    {
        m_pixel_grid.attach(
                    *m_pixels[xy.y() * mp_size.x() + xy.x()],
                    xy.x(),
                    xy.y(),
                    1,
                    1);
    }
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
    }
    else
    {
        m_pixel_format = Pixel_format();

        m_pixel_format.color_space = m_colorspace_frame.get_color_space();

        const Index planes_count =
                m_plane_frame.m_planes_count_entry.get_value_as_int();
        m_pixel_format.planes.resize(planes_count);
        for (Index i = 0; i < planes_count; ++i)
        {
            auto &plane_out = m_pixel_format.planes[i];
            const auto &plane_in = *m_plane_frame.m_planes[i];
            const Index rows_count = plane_in.m_rows.size();
            plane_out.rows.resize(rows_count);
            for (Index j = 0; j < rows_count; ++j)
            {
                auto &row_out = plane_out.rows[j];
                const auto &row_in = *plane_in.m_rows[j];
                const Index entries_count = row_in.m_entries.size();
                row_out.entries.resize(entries_count);
                for (Index k = 0; k < entries_count; ++k)
                {
                    auto &entry_out = row_out.entries[k];
                    const auto &entry_in = *row_in.m_entries[k];
                    entry_out.width =
                            entry_in.m_bit_count_entry.get_value_as_int();
                }
            }
        }

        auto &mp_size = m_pixel_format.macropixel_coding.size;
        mp_size.set(
                    m_macropixel_frame.m_columns_entry.get_value_as_int(),
                    m_macropixel_frame.m_rows_entry.get_value_as_int());
        m_pixel_format.macropixel_coding.pixels.resize(
                    mp_size.x() * mp_size.y());
        for (Index y = 0; y < mp_size.y(); ++y)
        {
            for (Index x = 0; x < mp_size.x(); ++x)
            {
                const Index i = y * mp_size.x() + x;
                auto &pixel_out = m_pixel_format.macropixel_coding.pixels[i];
                const auto &pixel_in = *m_macropixel_frame.m_pixels[i];
                const Index components_count = pixel_in.m_samples.size();
                pixel_out.components.resize(components_count);
                for (Index j = 0; j < components_count; ++j)
                {
                    auto &component_out = pixel_out.components[j];
                    const auto &component_in = *pixel_in.m_samples[j];
                    component_out.plane_index =
                            component_in.m_plane_entry.get_value_as_int();
                    component_out.row_index =
                            component_in.m_row_entry.get_value_as_int();
                    component_out.entry_index =
                            component_in.m_index_entry.get_value_as_int();
                }
            }
        }
    }
}
/*----------------------------------------------------------------------------*/
void Format_chooser_dialog::on_predefined_format()
{
    const Pixel_format *predefined_format =
            (*m_predefined_choice.get_active())[
                m_pixel_format_column_record.m_pointer];

    if(predefined_format != nullptr)
    {
        m_import_box.hide();
        m_plane_frame.hide();
        m_colorspace_frame.hide();
        m_macropixel_frame.hide();
    }
    else
    {
        m_import_box.show();
        m_plane_frame.show();
        m_colorspace_frame.show();
        m_macropixel_frame.show();
    }
    update_format();
}
/*----------------------------------------------------------------------------*/
void Format_chooser_dialog::on_import_format()
{
    const Pixel_format *import_format =
            (*m_import_box.m_choice.get_active())[
                m_pixel_format_column_record.m_pointer];

    if(import_format != nullptr)
    {
        m_colorspace_frame.set_color_space(import_format->color_space);

        const Index planes_count = import_format->planes.size();
        m_plane_frame.m_planes_count_entry.set_value(planes_count);
        m_plane_frame.set_planes_count(planes_count);
        for (Index i = 0; i < planes_count; ++i)
        {
            auto &plane_out = *m_plane_frame.m_planes[i];
            const auto &plane_in = import_format->planes[i];
            const Index rows_count = plane_in.rows.size();
            plane_out.m_row_count_entry.set_value(rows_count);
            plane_out.set_rows_count(rows_count);
            for (Index j = 0; j < rows_count; ++j)
            {
                auto &row_out = *plane_out.m_rows[j];
                const auto &row_in = plane_in.rows[j];
                const Index entries_count = row_in.entries.size();
                row_out.m_entry_count_entry.set_value(entries_count);
                row_out.set_entries_count(entries_count);
                for (Index k = 0; k < entries_count; ++k)
                {
                    auto &entry_out = *row_out.m_entries[k];
                    const auto &entry_in = row_in.entries[k];
                    entry_out.m_bit_count_entry.set_value(
                                entry_in.width.get_position());
                }
            }
        }
        m_plane_frame.show_all();

        const auto mp_size = import_format->macropixel_coding.size;
        m_macropixel_frame.m_columns_entry.set_value(mp_size.x());
        m_macropixel_frame.m_rows_entry.set_value(mp_size.y());
        m_macropixel_frame.reshape();

        for (Index y = 0; y < mp_size.y(); ++y)
        {
            for (Index x = 0; x < mp_size.x(); ++x)
            {
                const Index i = y * mp_size.x() + x;
                auto &pixel_out =
                        *m_macropixel_frame.m_pixels[i];;
                const auto &pixel_in = import_format->macropixel_coding.pixels[i];
                const Index components_count = pixel_in.components.size();
                pixel_out.set_samples_count(components_count);
                for (Index j = 0; j < components_count; ++j)
                {
                    auto &component_out = *pixel_out.m_samples[j];
                    const auto &component_in = pixel_in.components[j];
                    component_out.m_plane_entry.set_value(
                            component_in.plane_index);
                    component_out.m_row_entry.set_value(
                            component_in.row_index);
                    component_out.m_index_entry.set_value(
                            component_in.entry_index);
                }
            }
        }
    }

    m_import_box.m_choice.set_active(--m_import_list_store->children().end());

    update_format();
}
/*----------------------------------------------------------------------------*/
}
