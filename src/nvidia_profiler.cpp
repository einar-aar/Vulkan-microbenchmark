
#include "nvidia_profiler.hpp"

#include <string>
#include <vector>
#include <stdexcept>

#include <nvperf_host.h>

NvidiaProfiler::NvidiaProfiler() = default;
NvidiaProfiler::~NvidiaProfiler() = default;

void NvidiaProfiler::initialize() {
    NVPW_InitializeHost_Params params = { NVPW_InitializeHost_Params_STRUCT_SIZE };

    if (NVPW_InitializeHost(&params) != NVPA_STATUS_SUCCESS) {
        throw std::runtime_error("Failed to initialize NVIDIA Perf SDK host");
    }
}