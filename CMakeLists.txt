cmake_minimum_required(VERSION 3.20)
project(vulkan_microbenchmark)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Vulkan REQUIRED)

add_executable(vulkan_microbenchmark
    src/main.cpp
    src/vulkan_infrastructure.cpp
    src/vulkan_benchmark.cpp
)

target_include_directories(vulkan_microbenchmark PRIVATE headers)

target_link_libraries(vulkan_microbenchmark PRIVATE Vulkan::Vulkan)