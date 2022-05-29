project(gui)

find_package(PkgConfig REQUIRED)

pkg_check_modules(gtkmm-3.0 REQUIRED gtkmm-3.0 IMPORTED_TARGET)
pkg_check_modules(epoxy REQUIRED epoxy IMPORTED_TARGET)

add_executable(
    yuvtool
    color_space_frame.cpp
    color_space_frame.h
    drawer_gl.cpp
    drawer_gl.h
    format_chooser_widget.cpp
    format_chooser_widget.h
    main.cpp
    resolution_and_format_dialog.cpp
    resolution_and_format_dialog.h
    resolution_chooser_widget.cpp
    resolution_chooser_widget.h
    scroll_adapter.cpp
    scroll_adapter.h
    viewer_frame.cpp
    viewer_frame.h
    yuv_tool_application.cpp
    yuv_tool_application.h)

target_include_directories(yuvtool PRIVATE .)

target_link_libraries(
    yuvtool
    PRIVATE
        yuv
        PkgConfig::gtkmm-3.0
        PkgConfig::epoxy)

install(TARGETS yuvtool RUNTIME DESTINATION bin)
