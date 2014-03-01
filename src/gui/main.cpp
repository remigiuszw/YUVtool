#include <gtkmm/main.h>
#include "viewer_frame.h"

int main( int argc, char *argv[] )
{
    Gtk::Main kit( argc, argv );

    Viewer_frame viewer_frame;

    Gtk::Main::run( viewer_frame );

    return 0;
}
