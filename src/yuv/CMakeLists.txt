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
    TYUV_file.cpp
    TComponents.cpp
    Errors.cpp
    picture_buffer.cpp
)

set(HEADER_FILES
    TYUV_file.h
    TComponents.h
    Errors.h
    utils.h
    picture_buffer.h
    coordinates.h
)

add_library(yuv ${CPP_SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(yuv ${Boost_LIBRARIES})
