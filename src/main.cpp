
#include "vulkan_infrastructure.hpp"
#include "vulkan_benchmark.hpp"
#include <iostream>

int main() {

    try {

        VkInfrastructure infrastructure;

        VkPhysicalDevice physicalDev = infrastructure.getPhysicalDev();
        VkPhysicalDeviceProperties physicalDevProperties{};
        vkGetPhysicalDeviceProperties(physicalDev, &physicalDevProperties);

        uint32_t familyIndex = infrastructure.getComputeQueueFamilyIndex();

        std::cout << "Vulkan computation infrastructure has been successfully instantiated\n"
                  << "Infrastructure uses GPU: " << physicalDevProperties.deviceName << "\n"
                  << "Infrastructure uses queue family index: " << familyIndex << std::endl;

        VkBenchmark benchmark(infrastructure);

        benchmark.runBenchmark();

        std::cout << "Benchmark ran successfully" << std::endl;

    } catch (const std::exception& error) {

        std::cerr << "Program failed with error: " << error.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}