include_directories( ${YUVtool_SOURCE_DIR}/src/yuv )
link_directories( ${YUVtool_BINARY_DIR}/src/yuv )

find_package(wxWidgets REQUIRED gl core base)
include(${wxWidgets_USE_FILE})

set( CPP_SOURCE_FILES
  main.cpp
)

set( HEADER_FILES
)

add_executable( yuvtool ${CPP_SOURCE_FILES} ${HEADER_FILES} )

target_link_libraries( yuvtool yuv ${wxWidgets_LIBRARIES} )
