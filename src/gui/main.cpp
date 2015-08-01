#include <gtkmm/main.h>
#include "viewer_frame.h"

int main( int argc, char *argv[] )
{
    Glib::RefPtr<Gtk::Application> app =
            Gtk::Application::create(argc, argv, "org.yuvtool");

    Viewer_frame viewer_frame;

    return app->run(viewer_frame);
}
