/*
 * Copyright 2016 Dominik Wójt
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

#include <yuv/xml_metadata.h>
#include <yuv/Errors.h>

#include <tinyxml2.h>

namespace YUV_tool {
/*----------------------------------------------------------------------------*/
namespace  {
/*----------------------------------------------------------------------------*/
void check_tinyxml2_result(const tinyxml2::XMLError result)
{
    MY_ASSERT(result == tinyxml2::XML_SUCCESS);
}
/*----------------------------------------------------------------------------*/
template <typename Value>
Value get_xml_element_value(const tinyxml2::XMLElement *element);

template <>
int get_xml_element_value<int>(const tinyxml2::XMLElement *element)
{
    int i;
    auto status = element->QueryIntText(&i);
    check_tinyxml2_result(status);
    return i;
}

template <>
double get_xml_element_value<double>(const tinyxml2::XMLElement *element)
{
    double i;
    auto status = element->QueryDoubleText(&i);
    check_tinyxml2_result(status);
    return i;
}
/*----------------------------------------------------------------------------*/
template <typename Value>
Value get_xml_element_attribute(
        const tinyxml2::XMLElement *element,
        const char *name);

template <>
int get_xml_element_attribute<int>(
        const tinyxml2::XMLElement *element,
        const char *name)
{
    int i;
    auto status = element->QueryIntAttribute(name, &i);
    check_tinyxml2_result(status);
    return i;
}

template <>
double get_xml_element_attribute<double>(
        const tinyxml2::XMLElement *element,
        const char *name)
{
    double i;
    auto status = element->QueryDoubleAttribute(name, &i);
    check_tinyxml2_result(status);
    return i;
}
/*----------------------------------------------------------------------------*/
class XmlElementRange
{
public:
    class Iterator
    {
    private:
        const tinyxml2::XMLElement *m_element;
        const XmlElementRange &m_range;

    public:
        Iterator(const tinyxml2::XMLElement *element, XmlElementRange &range) :
            m_element(element),
            m_range(range)
        { }

        const tinyxml2::XMLElement *operator*()
        {
            return m_element;
        }

        Iterator &operator++()
        {
            m_element =
                    m_element->NextSiblingElement(m_range.m_child_name.c_str());
            return *this;
        }

        bool operator!=(const Iterator &rhs)
        {
            return m_element != rhs.m_element;
        }
    };

private:
    const tinyxml2::XMLNode *m_parent_node;
    const std::string m_child_name;

public:
    XmlElementRange(
            const tinyxml2::XMLNode *parent_node,
            const std::string &child_name) :
        m_parent_node(parent_node),
        m_child_name(child_name)
    { }

    Iterator begin()
    {
        return
                Iterator(
                    m_parent_node->FirstChildElement(m_child_name.c_str()),
                    *this);
    }

    Iterator end()
    {
        return Iterator(nullptr, *this);
    }
};
/*----------------------------------------------------------------------------*/
} /* anonymous namespace */
/*----------------------------------------------------------------------------*/
Pixel_format read_pixel_format(const std::string &file_name)
{
    tinyxml2::XMLDocument input_file;
    input_file.LoadFile(file_name.c_str());

    Pixel_format result;
    const tinyxml2::XMLNode *node = nullptr;

    node = input_file.FirstChild();
    MY_ASSERT(node);
    const auto *xml_declaration = node->ToDeclaration();
    MY_ASSERT(xml_declaration);

    const auto *pixel_format_element =
            input_file.FirstChildElement("pixel_format");
    MY_ASSERT(pixel_format_element);

    const auto *planes_element =
            pixel_format_element->FirstChildElement("planes");
    MY_ASSERT(planes_element);

    for(const auto *plane_element : XmlElementRange(planes_element, "plane"))
    {
        Plane plane;

        for(const auto *row_element : XmlElementRange(plane_element, "row"))
        {
            Entry_row row;

            for(
                const auto *entry_element :
                    XmlElementRange(row_element, "entry"))
            {
                Entry entry;
                entry.width = get_xml_element_value<int>(entry_element);
                row.entries.push_back(entry);
            }

            plane.rows.push_back(std::move(row));
        }

        result.planes.push_back(std::move(plane));
    }

    const auto *colorspace_elment =
            pixel_format_element->FirstChildElement("colorspace");
    MY_ASSERT(colorspace_elment);
    for(
        const auto component_element :
            XmlElementRange(colorspace_elment, "component"))
    {
        Component component;

        Index i = 0;
        for(
            const auto *coefficient_element :
                XmlElementRange(component_element, "coefficient"))
        {
            component.coeff[i++] =
                    get_xml_element_value<double>(coefficient_element);
        }

        const auto *valid_range_element =
                component_element->FirstChildElement("valid_range");
        MY_ASSERT(valid_range_element);
        component.valid_range[0] =
                get_xml_element_attribute<double>(valid_range_element, "min");
        component.valid_range[1] =
                get_xml_element_attribute<double>(valid_range_element, "max");

        const auto *coded_range_element =
                component_element->FirstChildElement("coded_range");
        MY_ASSERT(coded_range_element);
        component.coded_range[0] =
                get_xml_element_attribute<double>(coded_range_element, "min");
        component.coded_range[1] =
                get_xml_element_attribute<double>(coded_range_element, "max");

        result.color_space.components.push_back(std::move(component));
    }

    const auto *macropixel_coding_element =
            pixel_format_element->FirstChildElement("macropixel_coding");
    MY_ASSERT(macropixel_coding_element);

    result.macropixel_coding.size.set(
                get_xml_element_attribute<int>(
                    macropixel_coding_element,
                    "columns_count"),
                get_xml_element_attribute<int>(
                    macropixel_coding_element,
                    "rows_count"));

    for(
        const auto *coded_pixel_element :
            XmlElementRange(macropixel_coding_element, "coded_pixel"))
    {
        Coded_pixel coded_pixel;

        for(
            const auto *component_coding_element :
                XmlElementRange(coded_pixel_element, "component_coding"))
        {
            Component_coding component_coding;

            component_coding.plane_index =
                    get_xml_element_attribute<int>(
                        component_coding_element,
                        "plane_index");
            component_coding.row_index =
                    get_xml_element_attribute<int>(
                        component_coding_element,
                        "row_index");
            component_coding.entry_index =
                    get_xml_element_attribute<int>(
                        component_coding_element,
                        "entry_index");

            coded_pixel.components.push_back(std::move(component_coding));
        }

        result.macropixel_coding.pixels.push_back(std::move(coded_pixel));
    }

    MY_ASSERT(
                (
                    result.macropixel_coding.size.x()
                    * result.macropixel_coding.size.y())
                == static_cast<Index>(
                    result.macropixel_coding.pixels.size()));

    return result;
}
/*----------------------------------------------------------------------------*/
void store_pixel_format(
        const Pixel_format &pixel_format,
        const std::string &file_name)
{
    tinyxml2::XMLDocument output_file;

    tinyxml2::XMLDeclaration *declaration = output_file.NewDeclaration();
    output_file.InsertEndChild(declaration);

    tinyxml2::XMLNode *root = output_file.NewElement("pixel_format");
    output_file.InsertEndChild(root);

    /* planes */
    auto *planes_element = output_file.NewElement("planes");
    root->InsertEndChild(planes_element);

    for(auto &plane : pixel_format.planes)
    {
        auto *plane_element = output_file.NewElement("plane");
        planes_element->InsertEndChild(plane_element);

        for(auto &row : plane.rows)
        {
            auto *row_element = output_file.NewElement("row");
            plane_element->InsertEndChild(row_element);

            for(auto &entry : row.entries)
            {
                auto *entry_element = output_file.NewElement("entry");
                entry_element->SetText(entry.width.get_bits());
                row_element->InsertEndChild(entry_element);
            }
        }
    }

    /* colorspace */
    auto *colorspace_element = output_file.NewElement("colorspace");
    root->InsertFirstChild(colorspace_element);

    for(auto &component : pixel_format.color_space.components)
    {
        auto *component_element = output_file.NewElement("component");
        colorspace_element->InsertEndChild(component_element);

        for(auto &coefficient : component.coeff)
        {
            auto *coefficient_element = output_file.NewElement("coefficient");
            coefficient_element->SetText(to_string(coefficient).c_str());
            component_element->InsertEndChild(coefficient_element);
        }

        auto *valid_range_element = output_file.NewElement("valid_range");
        valid_range_element->SetAttribute(
                    "min",
                    to_string(component.valid_range[0]).c_str());
        valid_range_element->SetAttribute(
                    "max",
                    to_string(component.valid_range[1]).c_str());
        component_element->InsertEndChild(valid_range_element);

        auto *coded_range_element = output_file.NewElement("coded_range");
        coded_range_element->SetAttribute(
                    "min",
                    to_string(component.coded_range[0]).c_str());
        coded_range_element->SetAttribute(
                    "max",
                    to_string(component.coded_range[1]).c_str());
        component_element->InsertEndChild(coded_range_element);
    }

    auto *macropixel_coding_element =
            output_file.NewElement("macropixel_coding");
    macropixel_coding_element->SetAttribute(
                "columns_count",
                static_cast<int>(pixel_format.macropixel_coding.size.x()));
    macropixel_coding_element->SetAttribute(
                "rows_count",
                static_cast<int>(pixel_format.macropixel_coding.size.y()));
    root->InsertEndChild(macropixel_coding_element);

    for(auto &coded_pixel : pixel_format.macropixel_coding.pixels)
    {
        auto *coded_pixel_element = output_file.NewElement("coded_pixel");
        macropixel_coding_element->InsertEndChild(coded_pixel_element);

        for(auto &component_coding : coded_pixel.components)
        {
            auto *component_coding_element =
                    output_file.NewElement("component_coding");
            component_coding_element->SetAttribute(
                        "plane_index",
                        static_cast<int>(component_coding.plane_index));
            component_coding_element->SetAttribute(
                        "row_index",
                        static_cast<int>(component_coding.row_index));
            component_coding_element->SetAttribute(
                        "entry_index",
                        static_cast<int>(component_coding.entry_index));
            coded_pixel_element->InsertEndChild(component_coding_element);
        }
    }

    auto result = output_file.SaveFile(file_name.c_str());
    check_tinyxml2_result(result);
}
/*----------------------------------------------------------------------------*/
}
