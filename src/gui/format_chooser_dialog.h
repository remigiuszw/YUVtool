#ifndef FORMAT_CHOOSER_DIALOG_H
#define FORMAT_CHOOSER_DIALOG_H

#include <yuv/Pixel_format.h>

#include <gtkmm/dialog.h>
#include <gtkmm/combobox.h>
#include <gtkmm/frame.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/grid.h>
#include <gtkmm/liststore.h>

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

        Gtk::ComboBox m_import_choice;
        Gtk::Button m_import_button;
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
        void add_entry(Format_chooser_dialog &parent);
        void remove_entry(Format_chooser_dialog &parent);

        Gtk::Box m_entry_count_box;
        Gtk::Label m_row_label;
        Gtk::SpinButton m_entry_count_entry;

        std::vector<std::unique_ptr<Entry_configurator> > m_entries;
    };

    struct Plane_configurator : Gtk::Frame
    {
        Plane_configurator();
        void add_row(Format_chooser_dialog &parent);
        void remove_row(Format_chooser_dialog &parent);

        Gtk::Box m_box;

        Gtk::Box m_row_count_box;
        Gtk::Label m_row_count_label;
        Gtk::SpinButton m_row_count_entry;

        std::vector<std::unique_ptr<Row_in_plane_configurator> > m_rows;
    };

    struct Plane_frame : Gtk::Frame
    {
        Plane_frame();
        void add_plane(Format_chooser_dialog &parent);
        void remove_plane(Format_chooser_dialog &parent);

        Gtk::Box m_box;
        std::vector<std::unique_ptr<Plane_configurator> > m_planes;
    };

    struct Component_configurator : Gtk::Frame
    {
        Component_configurator();

        Gtk::Box m_box;

        Gtk::Box m_r_box;
        Gtk::Label m_r_label;
        Gtk::SpinButton m_r_entry;

        Gtk::Box m_g_box;
        Gtk::Label m_g_label;
        Gtk::SpinButton m_g_entry;

        Gtk::Box m_b_box;
        Gtk::Label m_b_label;
        Gtk::SpinButton m_b_entry;

        Gtk::Box m_a_box;
        Gtk::Label m_a_label;
        Gtk::SpinButton m_a_entry;
    };

    struct Colorspace_frame : Gtk::Frame
    {
        Colorspace_frame();

        Gtk::Box m_box;

        Gtk::Box m_predefined_colorspace_box;
        Gtk::Label m_predefined_colorspace_label;
        Gtk::ComboBox m_predefined_colorspace_entry;

        Gtk::Box m_component_count_box;
        Gtk::Label m_component_count_label;
        Gtk::SpinButton m_component_count_entry;

        Gtk::Box m_component_box;
        std::vector<std::unique_ptr<Component_configurator> > m_components;
    };

    struct Sample_configurator : Gtk::Box
    {
        Sample_configurator();

        Gtk::Label m_sample_label;
        Gtk::Label m_plane_label;
        Gtk::ComboBox m_plane_entry;
        Gtk::Label m_row_label;
        Gtk::ComboBox m_row_entry;
        Gtk::Label m_column_label;
        Gtk::ComboBox m_column_entry;
    };

    struct Pixel_configurator : Gtk::Frame
    {
        Pixel_configurator();

        Gtk::Box m_box;
        std::vector<std::unique_ptr<Sample_configurator> > m_samples;
    };

    struct Macropixel_frame : Gtk::Frame
    {
        Macropixel_frame();

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

    void import_format();
    void update_format();

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
