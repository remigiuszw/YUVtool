project(cli)

add_executable(yuvtool_cli)
target_sources(yuvtool_cli PRIVATE main.cpp)
target_link_libraries(yuvtool_cli PRIVATE yuv)

install(TARGETS yuvtool_cli RUNTIME DESTINATION bin)
