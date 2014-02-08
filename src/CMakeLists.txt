if( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
    set(CMAKE_CXX_FLAGS "-std=c++11 -pedantic -Wall -Werror -Wno-deprecated-register" )
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
elseif( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
    set(CMAKE_CXX_FLAGS "-std=c++11 -pedantic -Wall -Werror" )
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
elseif( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel" )
    # using Intel C++
elseif( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC" )
    # using Visual Studio C++
endif()

include_directories( "." )

add_subdirectory( cli )
add_subdirectory( gui )
add_subdirectory( yuv )
