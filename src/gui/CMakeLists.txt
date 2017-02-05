project(gui)

include_directories( ${YUVtool_SOURCE_DIR}/src/yuv )
link_directories( ${YUVtool_BINARY_DIR}/src/yuv )

find_package(PkgConfig REQUIRED)

pkg_check_modules(GTKMM3 REQUIRED gtkmm-3.0)
include_directories(${GTKMM3_INCLUDE_DIRS})
link_directories(${GTKMM3_LIBRARY_DIRS})
add_definitions(${GTKMM3_CFLAGS_OTHER})

pkg_check_modules(EPOXY REQUIRED epoxy)
include_directories(${EPOXY_INCLUDE_DIRS})
link_directories(${EPOXY_LIBRARY_DIRS})
add_definitions(${EPOXY_CFLAGS_OTHER})

find_package(
    Eigen3
    REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

set( CPP_SOURCE_FILES
    yuv_tool_application.cpp
    viewer_frame.cpp
    scroll_adapter.cpp
    drawer_gl.cpp
    main.cpp
    format_chooser_dialog.cpp
)

set( HEADER_FILES
    yuv_tool_application.h
    viewer_frame.h
    scroll_adapter.h
    drawer_gl.h
    format_chooser_dialog.h
)

add_executable(yuvtool ${CPP_SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(yuvtool yuv ${GTKMM3_LIBRARIES} ${EPOXY_LIBRARIES})
