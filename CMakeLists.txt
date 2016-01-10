cmake_minimum_required(VERSION 3.0)

project( YUVtool )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/" )

option( DOC "produce documentation" )

if( DOC )
  add_subdirectory( doc )
endif()

add_subdirectory( src )
