include_directories( ${YUVtool_SOURCE_DIR}/src/yuv )
link_directories( ${YUVtool_BINARY_DIR}/src/yuv )

find_package(PkgConfig REQUIRED)

pkg_check_modules(GTKMM3 REQUIRED gtkmm-3.0)
include_directories(${GTKMM3_INCLUDE_DIRS})
link_directories(${GTKMM3_LIBRARY_DIRS})
add_definitions(${GTKMM3_CFLAGS_OTHER})
set( LIBS "${GTKMM3_LIBRARIES}")

pkg_check_modules(GTKGLEXT REQUIRED gtkglext-3.0)
include_directories(${GTKGLEXT_INCLUDE_DIRS})
link_directories(${GTKGLEXT_LIBRARY_DIRS})
add_definitions(${GTKGLEXT_CFLAGS_OTHER})
set( LIBS "${LIBS} ${GTKGLEXT_LIBRARIES}")

set( CPP_SOURCE_FILES
  yuv_tool_application.cpp
  viewer_frame.cpp
  scroll_adapter.cpp
  main.cpp
)

set( HEADER_FILES
  yuv_tool_application.h
  viewer_frame.h
  scroll_adapter.h
)

add_executable( yuvtool ${CPP_SOURCE_FILES} ${HEADER_FILES} )

target_link_libraries( yuvtool yuv ${GTKMM3_LIBRARIES} ${GTKGLEXT_LIBRARIES} )
