project(tests)

enable_testing()
find_package(GTest REQUIRED)

add_executable(cache_test cache_test.cpp)
target_link_libraries(cache_test PRIVATE yuv GTest::GTest GTest::Main)
add_test(cache_test cache_test)

add_executable(saturable_fixed_test saturable_fixed_test.cpp)
target_link_libraries(saturable_fixed_test PRIVATE yuv)
add_test(saturable_fixed_test cache_test)

add_executable(xml_metadata_test xml_metadata_test.cpp)
target_link_libraries(xml_metadata_test PRIVATE yuv)
add_test(xml_metadata_test cache_test)
