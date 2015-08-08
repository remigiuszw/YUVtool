/* copied from https://github.com/LaurentGomila/SFML/wiki/Source%3A-GTK-SFML_widget#wiki-SFML_widgeth
 * public domain */

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

SFML_widget::SFML_widget(sf::VideoMode mode, int size_request)
{
    if(size_request<=0)
        size_request = std::max<int>(1, std::min<int>(mode.width, mode.height) / 2);

    set_size_request(size_request, size_request);

    set_has_window(false); // Makes this behave like an interal object rather then a parent window.
}

SFML_widget::~SFML_widget()
{ }

void SFML_widget::on_size_allocate(Gtk::Allocation& allocation)
{
    //Do something with the space that we have actually been given:
    //(We will not be given heights or widths less than we have requested, though
    //we might get more)

    this->set_allocation(allocation);

    if(m_ref_gdk_window)
    {
        m_ref_gdk_window->move_resize(allocation.get_x(),
                                    allocation.get_y(),
                                    allocation.get_width(),
                                    allocation.get_height() );
//        renderWindow.setSize(sf::Vector2u(allocation.get_width(),
//                                          allocation.get_height()));

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

    m_render_window.create(
            GET_WINDOW_HANDLE_FROM_GDK(m_ref_gdk_window->gobj()));

    signal_post_size_allocate()();

    return true;
}

sigc::signal<void> &SFML_widget::signal_post_size_allocate()
{
    return m_signal_post_size_allocate;
}

sf::RenderWindow &SFML_widget::render_window()
{
    return m_render_window;
}

void SFML_widget::display()
{
    if(m_ref_gdk_window)
    {
        m_render_window.display();
    }
}

void SFML_widget::invalidate()
{
    if(m_ref_gdk_window)
    {
        m_ref_gdk_window->invalidate(true);
    }
}

} /* namespace YUV_tool */
