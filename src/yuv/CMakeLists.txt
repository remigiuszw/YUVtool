project(yuv)

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

find_package(
    Boost
    REQUIRED
    COMPONENTS filesystem system)
include_directories(${Boost_INCLUDE_DIRS})

find_package(
    Eigen3
    REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

set(CPP_SOURCE_FILES
    Errors.cpp
    Picture_buffer.cpp
    Pixel_format.cpp
    Yuv_file.cpp
)

set(HEADER_FILES
    Cache.h
    Coordinates.h
    Errors.h
    Picture_buffer.h
    Pixel_format.h
    trees_and_heaps.h
    utils.h
    Yuv_file.h
)

add_library(yuv ${CPP_SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(yuv ${Boost_LIBRARIES})
