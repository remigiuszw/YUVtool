include_directories( ${YUVtool_SOURCE_DIR}/yuv )
link_directories( ${YUVtool_BINARY_DIR}/yuv )

set( CPP_SOURCE_FILES
  main.cpp
)

set( HEADER_FILES
)

add_executable( yuvtool ${CPP_SOURCE_FILES} ${HEADER_FILES} )

target_link_libraries( yuvtool yuv )
