if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "-std=c++17 -pedantic -Wall -Wno-deprecated-register")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "-std=c++17 -pedantic -Wall" )
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    # using Intel C++
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # using Visual Studio C++
endif()

option(BUILD_GTKMM_GUI "build Gtkmm gui" ON)

add_subdirectory(yuv)
add_subdirectory(cli)
add_subdirectory(tests)

if(BUILD_GTKMM_GUI)
    add_subdirectory(gui)
endif()

enable_testing()
add_test(cache_test cache_test)
