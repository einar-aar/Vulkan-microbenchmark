cmake_minimum_required(VERSION 3.20)
project(vulkan_microbench)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)

add_executable(vulkan_microbench
    src/main.cpp
    src/vulkan_infrastructure.cpp
    src/vulkan_benchmark.cpp
)

target_include_directories(vulkan_microbench PRIVATE headers)

target_link_libraries(vulkan_microbench PRIVATE Vulkan::Vulkan)