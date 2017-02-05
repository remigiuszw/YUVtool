project(tests)

find_package(
    Eigen3
    REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

add_executable(cache_test cache_test.cpp)

add_executable(saturable_fixed_test saturable_fixed_test.cpp)
target_link_libraries(saturable_fixed_test yuv)

add_executable(xml_metadata_test xml_metadata_test.cpp)
target_link_libraries(xml_metadata_test yuv)
