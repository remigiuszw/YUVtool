/* copied from https://github.com/SFML/SFML/wiki/Source%3A-GTK-SFMLWidget
 * public domain
 * see: https://github.com/SFML/SFML/wiki */

#include <gui/SFMLWidget.h>

#include <iostream>

// Tested on Linux Mint 12.4 and Windows 7
#if defined(SFML_SYSTEM_WINDOWS)

#include <gdk/gdkwin32.h>
#define GET_WINDOW_HANDLE_FROM_GDK GDK_WINDOW_HANDLE

#elif defined(SFML_SYSTEM_LINUX) || defined(SFML_SYSTEM_FREEBSD)

#include <gdk/gdkx.h>
#define GET_WINDOW_HANDLE_FROM_GDK GDK_WINDOW_XID

#elif defined(SFML_SYSTEM_MACOS)

#error Note: You have to figure out an analogue way to access the handle of the widget on a Mac-System

#else

#error Unsupported Operating System

#endif

namespace YUV_tool {

SFML_widget::SFML_widget()
{
    set_has_window(false); // Makes this behave like an interal object rather then a parent window.
}

SFML_widget::~SFML_widget()
{ }

void SFML_widget::on_size_allocate(Gtk::Allocation& allocation)
{
    this->set_allocation(allocation);

    if(m_ref_gdk_window)
    {
        m_ref_gdk_window->move_resize(allocation.get_x(),
                                    allocation.get_y(),
                                    allocation.get_width(),
                                    allocation.get_height() );

        if(m_timeout_source)
        {
            m_timeout_source->destroy();
            m_timeout_source.clear();
        }
        m_timeout_source = Glib::TimeoutSource::create(500);
        m_timeout_source->connect(
                sigc::mem_fun(*this, &SFML_widget::on_timeout));
        m_timeout_source->attach(Glib::MainContext::get_default());
    }
}

void SFML_widget::on_realize()
{
    Gtk::Widget::on_realize();

    on_realize_internal();
}

void SFML_widget::on_realize_internal()
{
    if(!m_ref_gdk_window)
    {
        //Create the GdkWindow:
        GdkWindowAttr attributes;
        memset(&attributes, 0, sizeof(attributes));

        Gtk::Allocation allocation = get_allocation();

        //Set initial position and size of the Gdk::Window:
        attributes.x = allocation.get_x();
        attributes.y = allocation.get_y();
        attributes.width = allocation.get_width();
        attributes.height = allocation.get_height();

        attributes.event_mask = get_events () | Gdk::EXPOSURE_MASK;
        attributes.window_type = GDK_WINDOW_CHILD;
        attributes.wclass = GDK_INPUT_OUTPUT;

        m_ref_gdk_window = Gdk::Window::create(get_window(), &attributes,
                GDK_WA_X | GDK_WA_Y);
        set_has_window(true);
        set_window(m_ref_gdk_window);

        // transparent background
#if GTK_VERSION_GE(3, 0)
        this->unset_background_color();
#else
        this->get_window()->set_back_pixmap(Glib::RefPtr<Gdk::Pixmap>());
#endif

        this->set_double_buffered(false);

        //make the widget receive expose events
        m_ref_gdk_window->set_user_data(gobj());

        m_render_window.create(
                GET_WINDOW_HANDLE_FROM_GDK(m_ref_gdk_window->gobj()));

        signal_post_size_allocate()();
    }
}

void SFML_widget::on_unrealize()
{
    on_unrealize_internal();

    //Call base class:
    Gtk::Widget::on_unrealize();
}

void SFML_widget::on_unrealize_internal()
{
    m_ref_gdk_window.clear();
}

bool SFML_widget::on_timeout()
{
    m_timeout_source->destroy();
    m_timeout_source.clear();

    if(m_ref_gdk_window)
    {
        m_render_window.create(
                    GET_WINDOW_HANDLE_FROM_GDK(m_ref_gdk_window->gobj()));
//        std::cerr << "SFML_widget::on_timeout()" << std::endl;
        signal_post_size_allocate()();
    }


    return true;
}

sigc::signal<void> &SFML_widget::signal_post_size_allocate()
{
    return m_signal_post_size_allocate;
}

bool SFML_widget::set_active(bool active)
{
    return m_render_window.setActive(active);
}

void SFML_widget::display()
{
    if(m_ref_gdk_window)
    {
        m_render_window.display();
    }
}

} /* namespace YUV_tool */
