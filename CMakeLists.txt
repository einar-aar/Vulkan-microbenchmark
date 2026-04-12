cmake_minimum_required(VERSION 3.20)
project(vulkan_microbenchmark)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(Vulkan_INCLUDE_DIR "C:/Users/einar/Vulkan/Vulkan SDK/Include")
set(Vulkan_LIBRARY "C:/Users/einar/Vulkan/Vulkan SDK/Lib/vulkan-1.lib")
set(NSIGHT_PERF_SDK_DIR "${CMAKE_SOURCE_DIR}/external/nsight-perf-sdk")

find_package(Vulkan REQUIRED)

add_executable(vulkan_microbenchmark
    src/main.cpp
    src/vulkan_infrastructure.cpp
    src/vulkan_benchmark.cpp
    src/nvidia_profiler.cpp
)

target_include_directories(vulkan_microbenchmark PRIVATE
    headers
    "${Vulkan_INCLUDE_DIR}"
    "${NSIGHT_PERF_SDK_DIR}/NvPerf/include"
)

file(GLOB_RECURSE NVPERF_LIB_FILES
    "${NSIGHT_PERF_SDK_DIR}/redist/*.lib"
    "${NSIGHT_PERF_SDK_DIR}/NvPerf/*.lib"
)

target_link_libraries(vulkan_microbenchmark PRIVATE
    ${Vulkan_LIBRARY}
    ${NVPERF_LIB_FILES}
)

file(GLOB_RECURSE NVPERF_DLL_FILES
    "${NSIGHT_PERF_SDK_DIR}/redist/*.dll"
    "${NSIGHT_PERF_SDK_DIR}/NvPerf/*.dll"
)

foreach(dll ${NVPERF_DLL_FILES})
    add_custom_command(TARGET vulkan_microbenchmark POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${dll}"
        $<TARGET_FILE_DIR:vulkan_microbenchmark>
    )
endforeach()