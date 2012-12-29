cmake_minimum_required( VERSION 2.8 )

project( YUVtool )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/" )

option( DOC "produce documentation" )

if( DOC )
  add_subdirectory( doc )
endif()

add_subdirectory( src )

#include_directories( "${PROJECT_SOURCE_DIR}/src" )

#find_package( Boost REQUIRED COMPONENTS filesystem system )
#include_directories( ${Boost_INCLUDE_DIRS} )
#set( LIBS ${LIBS} ${Boost_LIBRARIES} )

#find_package( GSL REQUIRED )
#include_directories( ${GSL_INCLUDE_DIRS} )
#set( LIBS ${LIBS} ${GSL_LIBRARIES} )

#find_package( Eigen3 REQUIRED )
#include_directories( ${EIGEN3_INCLUDE_DIR} )


#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#add_executable( MD
  #src/functions.cc
  #src/main.cc
  #src/TAtom.cc
  #src/TFunction.cc
  #src/TMolecular_dynamics.cc
  #src/TStructure.cc
  #src/TStructure_domain.cc
  #src/TCubic_interpolation/TCubic_interpolation.cc )
#target_link_libraries(MD ${LIBS})

