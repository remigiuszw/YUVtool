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
#include <colorspace_frame.h>

namespace YUV_tool {
/*----------------------------------------------------------------------------*/
Colorspace_frame::Color_group::Color_group(
        const std::string &name) :
    m_box(Gtk::ORIENTATION_HORIZONTAL),
    m_label(name),
    m_entry(Gtk::Adjustment::create(0, 0, 1, 0.01, 0.1), 0, 3)
{
    m_box.pack_start(m_label);
    m_box.pack_start(m_entry);
}
/*----------------------------------------------------------------------------*/
Colorspace_frame::Component_configurator::Component_configurator() :
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
    m_frame.add(m_box);
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

    m_frame.set_no_show_all(true);
}
/*----------------------------------------------------------------------------*/
Colorspace_frame::Color_space_column_record::Color_space_column_record()
{
    add(label);
    add(pointer);
}
/*----------------------------------------------------------------------------*/
Colorspace_frame::Colorspace_frame() :
    m_box(Gtk::ORIENTATION_VERTICAL),
    m_predefined_box(Gtk::ORIENTATION_HORIZONTAL),
    m_predefined_label("colorspace:"),
    m_component_count_box(Gtk::ORIENTATION_HORIZONTAL),
    m_component_count_label("components_count:"),
    m_component_count_entry(
        Gtk::Adjustment::create(0, 0, Rgba_component_count, 1)),
    m_component_box()
{
    set_label("Color space");
    set_shadow_type(Gtk::SHADOW_ETCHED_OUT);

    add(m_box);
    m_box.pack_start(m_predefined_box);
    m_predefined_box.pack_start(m_predefined_label);
    m_predefined_box.pack_start(m_predefined_entry);
    m_box.pack_start(m_component_count_box);
    m_component_count_box.pack_start(m_component_count_label);
    m_component_count_box.pack_start(m_component_count_entry);
    m_box.pack_start(m_component_box);
    for (auto & component : m_components)
        m_component_box.append_page(component.m_frame);

    m_predefined_list_store =
            Gtk::ListStore::create(m_predefined_column_record);
    Gtk::ListStore::iterator iter;
    iter = m_predefined_list_store->append();
    (*iter)[m_predefined_column_record.label] = "RGB";
    (*iter)[m_predefined_column_record.pointer] = &RGB_color_space;
    iter = m_predefined_list_store->append();
    (*iter)[m_predefined_column_record.label] = "sRGB";
    (*iter)[m_predefined_column_record.pointer] = &sRGB_color_space;
    iter = m_predefined_list_store->append();
    (*iter)[m_predefined_column_record.label] = "ITU601_YCbCr";
    (*iter)[m_predefined_column_record.pointer] =
            &ITU601_YCbCr_color_space;
    iter = m_predefined_list_store->append();
    (*iter)[m_predefined_column_record.label] = "ITU709_YCbCr";
    (*iter)[m_predefined_column_record.pointer] =
            &ITU709_YCbCr_color_space;
    iter = m_predefined_list_store->append();
    (*iter)[m_predefined_column_record.label] = "ITU2020_YCbCr";
    (*iter)[m_predefined_column_record.pointer] =
            &ITU2020_YCbCr_color_space;
    iter = m_predefined_list_store->append();
    (*iter)[m_predefined_column_record.label] = "custom";
    (*iter)[m_predefined_column_record.pointer] = nullptr;
    m_predefined_entry.set_model(m_predefined_list_store);
    m_predefined_entry.pack_start(m_predefined_column_record.label);
    m_predefined_entry.set_active(iter);

    const auto update_handler = sigc::mem_fun(this, &Colorspace_frame::update);
    m_predefined_entry.signal_changed().connect(update_handler);
    m_component_count_entry.signal_value_changed().connect(update_handler);
    for (auto &component : m_components)
    {
        for (auto &color : component.m_colors)
        {
            color.m_entry.signal_value_changed().connect(update_handler);
        }

        component.m_valid_range_low_entry.signal_value_changed().connect(
            update_handler);
        component.m_valid_range_high_entry.signal_value_changed().connect(
            update_handler);
        component.m_valid_range_low_entry.signal_value_changed().connect(
            update_handler);
        component.m_valid_range_high_entry.signal_value_changed().connect(
            update_handler);
    }

    update();
}
/*----------------------------------------------------------------------------*/
Color_space Colorspace_frame::get_color_space() const
{
    Color_space result;

    const Color_space* predefined_cs =
        (*m_predefined_entry.get_active())[m_predefined_column_record.pointer];
    if (predefined_cs != nullptr)
    {
        result = *predefined_cs;
    }
    else
    {
        const Index components_count =
            m_component_count_entry.get_value_as_int();
        result.components.resize(components_count);

        for (Index i = 0; i < components_count; ++i)
        {
            auto &component = m_components[i];
            for (Index j = 0; j < Rgba_component_count; ++j)
            {
                result.components[i].coeff[j] =
                        component.m_colors[j].m_entry.get_value_as_int();
            }
            result.components[i].valid_range[0] =
                    saturable_fixed::from_double(
                        component.m_valid_range_low_entry.get_value());
            result.components[i].valid_range[1] =
                    saturable_fixed::from_double(
                        component.m_valid_range_high_entry.get_value());
            result.components[i].coded_range[0] =
                    saturable_fixed::from_double(
                        component.m_coded_range_low_entry.get_value());
            result.components[i].coded_range[1] =
                    saturable_fixed::from_double(
                        component.m_coded_range_high_entry.get_value());
        }
    }

    return result;
}
/*----------------------------------------------------------------------------*/
void Colorspace_frame::set_color_space(const Color_space &color_space)
{
    if (!m_update_in_progress && get_color_space() == color_space)
        return;

    const auto predefined_container = m_predefined_list_store->children();
    const auto predefined_iter = std::find_if(
        predefined_container.begin(),
        predefined_container.end(),
        [&color_space, this](const auto& x) {
            const Color_space* const predefined_cs =
                x[m_predefined_column_record.pointer];
            return predefined_cs && *predefined_cs == color_space;
        });
    if (predefined_iter == predefined_container.end())
    {
        m_predefined_entry.set_active(--predefined_container.end());
        m_component_box.set_sensitive(true);
    }
    else
    {
        m_predefined_entry.set_active(predefined_iter);
        m_component_box.set_sensitive(false);
    }

    update_components(color_space);

    signal_color_space_changed().emit();
}
/*----------------------------------------------------------------------------*/
sigc::signal<void()> &
    Colorspace_frame::signal_color_space_changed()
{
    return m_signal_color_space_changed;
}
/*----------------------------------------------------------------------------*/
void Colorspace_frame::update()
{
    if (m_update_in_progress)
        return;

    m_update_in_progress = true;
    set_color_space(get_color_space());
    m_update_in_progress = false;
}
/*----------------------------------------------------------------------------*/
void Colorspace_frame::update_components(const Color_space &color_space)
{
    const Index components_count = color_space.components.size();
    m_component_count_entry.set_value(components_count);

    for (Index i = 0; i < Rgba_component_count; ++i)
    {
        auto &component_out = m_components[i];
        if (i < components_count)
        {
            const auto &component_in = color_space.components[i];
            for (Index j = 0; j < Rgba_component_count; ++j)

            {
                component_out.m_colors[j].m_entry.set_value(
                            component_in.coeff[j].to_double());
            }
            component_out.m_valid_range_low_entry.set_value(
                        component_in.valid_range[0].to_double());
            component_out.m_valid_range_high_entry.set_value(
                        component_in.valid_range[1].to_double());
            component_out.m_coded_range_low_entry.set_value(
                        component_in.valid_range[0].to_double());
            component_out.m_coded_range_high_entry.set_value(
                        component_in.valid_range[1].to_double());
            component_out.m_frame.show();
        }
        else
        {
            component_out.m_frame.hide();
        }
    }
}
/*----------------------------------------------------------------------------*/
} /* namespace YUV_tool */
