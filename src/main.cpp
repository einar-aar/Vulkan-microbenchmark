
#include "vulkan_infrastructure.hpp"
#include <iostream>

int main() {

    try {

        VkInfrastructure infrastructure;

        VkPhysicalDevice physicalDev = infrastructure.getPhysicalDev();
        VkPhysicalDeviceProperties physicalDevProperties{};
        vkGetPhysicalDeviceProperties(physicalDev, &physicalDevProperties);

        uint32_t familyIndex = infrastructure.getComputeQueueFamilyIndex();

        std::cout << "Vulkan computation infrastructure has been successfully instanciated\n"
                  << "Infrastructure uses GPU: " << physicalDevProperties.deviceName << "\n"
                  << "Infrastructure uses queue family index: " << familyIndex << std::endl;

    } catch (const std::exception& error) {

        std::cerr << "Infrastructure creation failed with error: " << error.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}