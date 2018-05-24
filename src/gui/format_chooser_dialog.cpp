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

    /* color space */
    {
        m_color_space_list_store =
                Gtk::ListStore::create(m_color_space_column_record);
        Gtk::ListStore::iterator iter;
        iter = m_color_space_list_store->append();
        (*iter)[m_color_space_column_record.m_label] = "RGB";
        (*iter)[m_color_space_column_record.m_pointer] = &RGB_color_space;
        iter = m_color_space_list_store->append();
        (*iter)[m_color_space_column_record.m_label] = "sRGB";
        (*iter)[m_color_space_column_record.m_pointer] = &sRGB_color_space;
        iter = m_color_space_list_store->append();
        (*iter)[m_color_space_column_record.m_label] = "ITU601_YCbCr";
        (*iter)[m_color_space_column_record.m_pointer] =
                &ITU601_YCbCr_color_space;
        iter = m_color_space_list_store->append();
        (*iter)[m_color_space_column_record.m_label] = "ITU709_YCbCr";
        (*iter)[m_color_space_column_record.m_pointer] =
                &ITU709_YCbCr_color_space;
        iter = m_color_space_list_store->append();
        (*iter)[m_color_space_column_record.m_label] = "ITU2020_YCbCr";
        (*iter)[m_color_space_column_record.m_pointer] =
                &ITU2020_YCbCr_color_space;
        iter = m_color_space_list_store->append();
        (*iter)[m_color_space_column_record.m_label] = "custom";
        (*iter)[m_color_space_column_record.m_pointer] = nullptr;
        m_colorspace_frame.m_predefined_colorspace_entry.set_model(
                    m_color_space_list_store);
        m_colorspace_frame.m_predefined_colorspace_entry.pack_start(
                    m_color_space_column_record.m_label);
        m_colorspace_frame.m_predefined_colorspace_entry.set_active(iter);
    }

    m_predefined_choice.signal_changed().connect(
                sigc::mem_fun(
                    *this,
                    &Format_chooser_dialog::on_predefined_format));
    m_import_box.m_choice.signal_changed().connect(
                sigc::mem_fun(
                    *this,
                    &Format_chooser_dialog::on_import_format));

    auto update_format_handler =
            sigc::mem_fun(
                *this,
                &Format_chooser_dialog::on_predefined_format);
    m_colorspace_frame.m_component_count_entry.signal_changed().connect(
                update_format_handler);
    for (auto & component : m_colorspace_frame.m_components)
    {
        for (auto & color : component.m_colors)
            color.m_entry.signal_changed().connect(update_format_handler);
        component.m_coded_range_low_entry.signal_changed().connect(
                    update_format_handler);
        component.m_coded_range_high_entry.signal_changed().connect(
                    update_format_handler);
        component.m_valid_range_low_entry.signal_changed().connect(
                    update_format_handler);
        component.m_valid_range_high_entry.signal_changed().connect(
                    update_format_handler);
    }

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
Format_chooser_dialog::Component_configurator::Color_group::Color_group(
        const std::string &name) :
    m_box(Gtk::ORIENTATION_HORIZONTAL),
    m_label(name),
    m_entry(Gtk::Adjustment::create(0, 0, 1, 0.01, 0.1), 0, 3)
{
    m_box.pack_start(m_label);
    m_box.pack_start(m_entry);
}
/*----------------------------------------------------------------------------*/
Format_chooser_dialog::Component_configurator::Component_configurator() :
    m_box(Gtk::ORIENTATION_VERTICAL),
    m_colors{
        std::string("R"),
        std::string("G"),
        std::string("B"),
        std::string("A")},
    m_valid_range_box(Gtk::ORIENTATION_HORIZONTAL),
    m_valid_range_label("valid range:"),
    m_valid_range_low_entry(
        Gtk::Adjustment::create(0, -1000, 1000, 0.001),
        0,
        3),
    m_valid_range_high_entry(
        Gtk::Adjustment::create(0, -1000, 1000, 0.001),
        0,
        3),
    m_coded_range_box(Gtk::ORIENTATION_HORIZONTAL),
    m_coded_range_label("coded range:"),
    m_coded_range_low_entry(
        Gtk::Adjustment::create(0, -1000, 1000, 0.001),
        0,
        3),
    m_coded_range_high_entry(
        Gtk::Adjustment::create(0, -1000, 1000, 0.001),
        0,
        3)
{
    add(m_box);
    for (auto &color : m_colors)
        m_box.pack_start(color.m_box);
    m_box.pack_start(m_valid_range_box);
    m_valid_range_box.pack_start(m_valid_range_label);
    m_valid_range_box.pack_start(m_valid_range_low_entry);
    m_valid_range_box.pack_start(m_valid_range_high_entry);
    m_box.pack_start(m_coded_range_box);
    m_coded_range_box.pack_start(m_coded_range_label);
    m_coded_range_box.pack_start(m_coded_range_low_entry);
    m_coded_range_box.pack_start(m_coded_range_high_entry);
}
/*----------------------------------------------------------------------------*/
Format_chooser_dialog::Colorspace_frame::Colorspace_frame() :
    m_box(Gtk::ORIENTATION_VERTICAL),
    m_predefined_colorspace_box(Gtk::ORIENTATION_HORIZONTAL),
    m_predefined_colorspace_label("colorspace:"),
    m_component_count_box(Gtk::ORIENTATION_HORIZONTAL),
    m_component_count_label("components_count:"),
    m_component_count_entry(
        Gtk::Adjustment::create(0, 0, Rgba_component_count, 1)),
    m_component_box(Gtk::ORIENTATION_HORIZONTAL)
{
    set_label("Color space");
    set_shadow_type(Gtk::SHADOW_ETCHED_OUT);

    add(m_box);
    m_box.pack_start(m_predefined_colorspace_box);
    m_predefined_colorspace_box.pack_start(m_predefined_colorspace_label);
    m_predefined_colorspace_box.pack_start(m_predefined_colorspace_entry);
    m_box.pack_start(m_component_count_box);
    m_component_count_box.pack_start(m_component_count_label);
    m_component_count_box.pack_start(m_component_count_entry);
    m_box.pack_start(m_component_box);
    for (auto & component : m_components)
        m_component_box.pack_start(component);
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

        const auto &cs_in = m_colorspace_frame;
        auto &cs_out = m_pixel_format.m_color_space;
        const Color_space *predefined_cs =
                (*cs_in.m_predefined_colorspace_entry.get_active())[
                    m_color_space_column_record.m_pointer];
        if (predefined_cs != nullptr)
        {
            cs_out = *predefined_cs;
        }
        else
        {
            const Index components_count =
                    cs_in.m_component_count_entry.get_value_as_int();
            cs_out.m_components.resize(components_count);

            for (Index i = 0; i < components_count; ++i)
            {
                auto &component = cs_in.m_components[i];
                for (Index j = 0; j < Rgba_component_count; ++j)
                    cs_out.m_components[i].m_coeff[j] =
                            component.m_colors[j].m_entry.get_value_as_int();
                cs_out.m_components[i].m_valid_range[0] =
                        saturable_fixed::from_double(
                            component.m_valid_range_low_entry.get_value());
                cs_out.m_components[i].m_valid_range[1] =
                        saturable_fixed::from_double(
                            component.m_valid_range_high_entry.get_value());
                cs_out.m_components[i].m_coded_range[0] =
                        saturable_fixed::from_double(
                            component.m_coded_range_low_entry.get_value());
                cs_out.m_components[i].m_coded_range[1] =
                        saturable_fixed::from_double(
                            component.m_coded_range_high_entry.get_value());
            }
        }

        const Index planes_count =
                m_plane_frame.m_planes_count_entry.get_value_as_int();
        m_pixel_format.m_planes.resize(planes_count);
        for (Index i = 0; i < planes_count; ++i)
        {
            auto &plane_out = m_pixel_format.m_planes[i];
            const auto &plane_in = *m_plane_frame.m_planes[i];
            const Index rows_count = plane_in.m_rows.size();
            plane_out.m_rows.resize(rows_count);
            for (Index j = 0; j < rows_count; ++j)
            {
                auto &row_out = plane_out.m_rows[j];
                const auto &row_in = *plane_in.m_rows[j];
                const Index entries_count = row_in.m_entries.size();
                row_out.m_entries.resize(entries_count);
                for (Index k = 0; k < entries_count; ++k)
                {
                    auto &entry_out = row_out.m_entries[k];
                    const auto &entry_in = *row_in.m_entries[k];
                    entry_out.m_width =
                            entry_in.m_bit_count_entry.get_value_as_int();
                }
            }
        }

        auto &mp_size = m_pixel_format.m_macropixel_coding.m_size;
        mp_size.set(
                    m_macropixel_frame.m_columns_entry.get_value_as_int(),
                    m_macropixel_frame.m_rows_entry.get_value_as_int());
        m_pixel_format.m_macropixel_coding.m_pixels.resize(
                    mp_size.x() * mp_size.y());
        for (Index y = 0; y < mp_size.y(); ++y)
        {
            for (Index x = 0; x < mp_size.x(); ++x)
            {
                const Index i = y * mp_size.x() + x;
                auto &pixel_out =
                        m_pixel_format.m_macropixel_coding.m_pixels[i];
                const auto &pixel_in = *m_macropixel_frame.m_pixels[i];
                const Index components_count = pixel_in.m_samples.size();
                pixel_out.m_components.resize(components_count);
                for (Index j = 0; j < components_count; ++j)
                {
                    auto &component_out = pixel_out.m_components[j];
                    const auto &component_in = *pixel_in.m_samples[j];
                    component_out.m_plane_index =
                            component_in.m_plane_entry.get_value_as_int();
                    component_out.m_row_index =
                            component_in.m_row_entry.get_value_as_int();
                    component_out.m_entry_index =
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
        const auto &cs_in = import_format->m_color_space;
        auto &cs_out = m_colorspace_frame;
        cs_out.m_predefined_colorspace_entry.set_active(
                    --m_color_space_list_store->children().end());

        const Index components_count = cs_in.m_components.size();
        cs_out.m_component_count_entry.set_value(components_count);

        for (Index i = 0; i < Rgba_component_count; ++i)
        {
            auto &component_out = cs_out.m_components[i];
            if (i < components_count)
            {
                const auto &component_in = cs_in.m_components[i];
                for (Index j = 0; j < Rgba_component_count; ++j)
                    component_out.m_colors[j].m_entry.set_value(
                                component_in.m_coeff[j].to_double());
                component_out.m_valid_range_low_entry.set_value(
                            component_in.m_valid_range[0].to_double());
                component_out.m_valid_range_high_entry.set_value(
                            component_in.m_valid_range[1].to_double());
                component_out.m_coded_range_low_entry.set_value(
                            component_in.m_valid_range[0].to_double());
                component_out.m_coded_range_high_entry.set_value(
                            component_in.m_valid_range[1].to_double());
                component_out.show();
            }
            else
            {
                component_out.hide();
            }
        }

        const Index planes_count = import_format->m_planes.size();
        m_plane_frame.m_planes_count_entry.set_value(planes_count);
        m_plane_frame.set_planes_count(planes_count);
        for (Index i = 0; i < planes_count; ++i)
        {
            auto &plane_out = *m_plane_frame.m_planes[i];
            const auto &plane_in = import_format->m_planes[i];
            const Index rows_count = plane_in.m_rows.size();
            plane_out.m_row_count_entry.set_value(rows_count);
            plane_out.set_rows_count(rows_count);
            for (Index j = 0; j < rows_count; ++j)
            {
                auto &row_out = *plane_out.m_rows[j];
                const auto &row_in = plane_in.m_rows[j];
                const Index entries_count = row_in.m_entries.size();
                row_out.m_entry_count_entry.set_value(entries_count);
                row_out.set_entries_count(entries_count);
                for (Index k = 0; k < entries_count; ++k)
                {
                    auto &entry_out = *row_out.m_entries[k];
                    const auto &entry_in = row_in.m_entries[k];
                    entry_out.m_bit_count_entry.set_value(
                                entry_in.m_width.get_position());
                }
            }
        }
        m_plane_frame.show_all();

        const auto mp_size = import_format->m_macropixel_coding.m_size;
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
                const auto &pixel_in = import_format->m_macropixel_coding.m_pixels[i];
                const Index components_count = pixel_in.m_components.size();
                pixel_out.set_samples_count(components_count);
                for (Index j = 0; j < components_count; ++j)
                {
                    auto &component_out = *pixel_out.m_samples[j];
                    const auto &component_in = pixel_in.m_components[j];
                    component_out.m_plane_entry.set_value(
                            component_in.m_plane_index);
                    component_out.m_row_entry.set_value(
                            component_in.m_row_index);
                    component_out.m_index_entry.set_value(
                            component_in.m_entry_index);
                }
            }
        }
    }

    m_import_box.m_choice.set_active(--m_import_list_store->children().end());

    update_format();
}
/*----------------------------------------------------------------------------*/
}
