project(yuv)

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)

find_package(Boost REQUIRED COMPONENTS filesystem system)
find_package(Eigen3 REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(tinyxml2 REQUIRED tinyxml2 IMPORTED_TARGET)


add_library(
    yuv
    Errors.cpp
    Picture_buffer.cpp
    Pixel_format.cpp
    saturable_fixed.cpp
    Yuv_file.cpp
    xml_metadata.cpp
    include/yuv/Cache.h
    include/yuv/Coordinates.h
    include/yuv/Errors.h
    include/yuv/saturable_fixed.h
    include/yuv/Picture_buffer.h
    include/yuv/Pixel_format.h
    include/yuv/saturable_fixed.h
    include/yuv/trees_and_heaps.h
    include/yuv/utils.h
    include/yuv/xml_metadata.h
    include/yuv/Yuv_file.h)

target_include_directories(
    yuv
    PUBLIC
        include
    PRIVATE
        .)

target_link_libraries(
    yuv
    PUBLIC
        Boost::filesystem
        Boost::system
        Eigen3::Eigen
        PkgConfig::tinyxml2)
