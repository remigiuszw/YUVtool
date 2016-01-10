project(cli)

include_directories( ${YUVtool_SOURCE_DIR}/src/yuv )
link_directories( ${YUVtool_BINARY_DIR}/src/yuv )

set( CPP_SOURCE_FILES
  main.cpp
)

set( HEADER_FILES
)

add_executable( yuvtool_cli ${CPP_SOURCE_FILES} ${HEADER_FILES} )

target_link_libraries( yuvtool_cli yuv )
