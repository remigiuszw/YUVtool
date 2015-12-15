include_directories( ${YUVtool_SOURCE_DIR}/src/yuv )
link_directories( ${YUVtool_BINARY_DIR}/src/yuv )

find_package(PkgConfig REQUIRED)

pkg_check_modules(GTKMM3 REQUIRED gtkmm-3.0)
include_directories(${GTKMM3_INCLUDE_DIRS})
link_directories(${GTKMM3_LIBRARY_DIRS})
add_definitions(${GTKMM3_CFLAGS_OTHER})
set( LIBS "${GTKMM3_LIBRARIES}")

add_definitions( -DGL_GLEXT_PROTOTYPES )

find_package(SFML 2.1 REQUIRED system window graphics)
include_directories(${SFML_INCLUDE_DIR})

set( CPP_SOURCE_FILES
    yuv_tool_application.cpp
    viewer_frame.cpp
    scroll_adapter.cpp
    drawer_gl.cpp
    main.cpp
    SFMLWidget.cpp
    format_chooser_dialog.cpp
)

set( HEADER_FILES
    yuv_tool_application.h
    viewer_frame.h
    scroll_adapter.h
    drawer_gl.h
    SFMLWidget.h
    format_chooser_dialog.h
)

add_executable( yuvtool ${CPP_SOURCE_FILES} ${HEADER_FILES} )

target_link_libraries( yuvtool yuv ${GTKMM3_LIBRARIES} ${SFML_LIBRARIES} GL )

