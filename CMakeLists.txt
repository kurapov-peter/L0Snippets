cmake_minimum_required(VERSION 3.0)
project(L0Snippets)

add_executable(gpu_avail_check gpu_discover.cpp)
target_link_libraries(gpu_avail_check PRIVATE ze_loader)