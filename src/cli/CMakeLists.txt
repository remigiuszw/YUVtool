project(cli)

add_executable(yuvtool_cli main.cpp)
target_link_libraries(yuvtool_cli PRIVATE yuv)
