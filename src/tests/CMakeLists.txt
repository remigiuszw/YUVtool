project(tests)

enable_testing()
find_package(GTest REQUIRED)

add_executable(
    yuv_test
    cache_test.cpp
    saturable_fixed_test.cpp
    xml_metadata_test.cpp
    color_transform_test.cpp)
target_link_libraries(yuv_test PRIVATE yuv GTest::GTest GTest::Main)
add_test(yuv_test yuv_test)
