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
    format_chooser_dialog.cpp
    format_chooser_dialog.h
    main.cpp
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
