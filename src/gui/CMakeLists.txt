project(gui)

find_package(PkgConfig REQUIRED)

pkg_check_modules(gtkmm-3.0 REQUIRED gtkmm-3.0 IMPORTED_TARGET)
pkg_check_modules(epoxy REQUIRED epoxy IMPORTED_TARGET)

add_executable(
    yuvtool
    yuv_tool_application.cpp
    viewer_frame.cpp
    scroll_adapter.cpp
    drawer_gl.cpp
    main.cpp
    format_chooser_dialog.cpp
    yuv_tool_application.h
    viewer_frame.h
    scroll_adapter.h
    drawer_gl.h
    format_chooser_dialog.h)

target_include_directories(yuvtool PRIVATE .)

target_link_libraries(
    yuvtool
    PRIVATE
        yuv
        PkgConfig::gtkmm-3.0
        PkgConfig::epoxy)
