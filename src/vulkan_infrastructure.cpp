
#include "vulkan_infrastructure.hpp"

#include <vector>
#include <stdexcept>

// Constructor
VkInfrastructure::VkInfrastructure() {

    createInstance();
    choosePhysicalDev();
    createLogicalDev();
    createCmdPool();
}

// Destructor
VkInfrastructure::~VkInfrastructure() {

    // Destroy commandpool, logical device and instance if not Null pointer
    if (cmdPool != VK_NULL_HANDLE) vkDestroyCommandPool(dev, cmdPool, nullptr);
    if (dev != VK_NULL_HANDLE) vkDestroyDevice(dev, nullptr);
    if (instance != VK_NULL_HANDLE) vkDestroyInstance(instance, nullptr);
}

void VkInfrastructure::choosePhysicalDev() {

    // Find how many physical devices, used to initialize vector to store devices
    uint32_t devCount = 0;
    vkEnumeratePhysicalDevices(instance, &devCount, nullptr);

    if (devCount == 0) throw std::runtime_error("No GPUs with Vulkan support found");

    // Store all physical devices in a vector
    std::vector<VkPhysicalDevice> devs(devCount);
    vkEnumeratePhysicalDevices(instance, &devCount, devs.data());

    // Iterate through all physical devices found
    for (const auto& devIt : devs) {

        if (!checkDevSuitability(devIt)) continue;

        VkPhysicalDeviceProperties devProperties{};
        vkGetPhysicalDeviceProperties(devIt, &devProperties);

        // Choose first suitable discrete GPU available
        if (devProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            physicalDev = devIt;
            return;
        }

        // If no discrete available, choose whatever the system finds that is suitable
        physicalDev = devIt;
    }

    // Error if no suitable GPU found
    if (physicalDev == VK_NULL_HANDLE) throw std::runtime_error("No GPU with compute family queue found");
}

VkInfrastructure::QueueFamilyIndex VkInfrastructure::getQueueFamilies(VkPhysicalDevice dev) const {

    QueueFamilyIndex familyIndex;

    // Find how many queue families, used to initialize vector to store queue families
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueFamilyCount, nullptr);

    if (queueFamilyCount == 0) throw std::runtime_error("No queue families found");

    // Store all family properties in a vector
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueFamilyCount, queueFamilies.data());

    // Iterate through all queue families, and find first that allows compute commands
    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            familyIndex.familyIndex = i;
            break;
        }
    }
    return familyIndex;
}

// Helper function to check if physical device has compute family queues
bool VkInfrastructure::checkDevSuitability(VkPhysicalDevice dev) const {
    QueueFamilyIndex index = getQueueFamilies(dev);
    return index.hasValue();
}

void VkInfrastructure::createLogicalDev() {

    QueueFamilyIndex familyIndex = getQueueFamilies(physicalDev);

    if (!familyIndex.hasValue()) throw std::runtime_error("No queue family with compute functionality");

    float queuePriority = 1.0f;

    // Create information about queues
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfo.queueFamilyIndex = familyIndex.familyIndex.value();

    // Create information about logical device
    VkDeviceCreateInfo createLogicalDevInfo{};
    createLogicalDevInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createLogicalDevInfo.enabledExtensionCount = 0;
    createLogicalDevInfo.ppEnabledExtensionNames = nullptr;
    createLogicalDevInfo.enabledLayerCount = 0;
    createLogicalDevInfo.ppEnabledLayerNames = nullptr;
    createLogicalDevInfo.queueCreateInfoCount = 1;
    createLogicalDevInfo.pQueueCreateInfos = &queueCreateInfo;
    createLogicalDevInfo.pEnabledFeatures = nullptr;
    
    // Check if creation of logical device is successful
    if (vkCreateDevice(physicalDev, &createLogicalDevInfo, nullptr, &dev) != VK_SUCCESS) {
        throw std::runtime_error("Creation of logical device failed");
    }

    // Store the index of the compute family in private variable of vulkan infrastructure object
    vkGetDeviceQueue(dev, familyIndex.familyIndex.value(), 0, &queue);
}

// Setting up instance
void VkInfrastructure::createInstance() {

    // Info about application
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.apiVersion = VK_API_VERSION_1_3;
    appInfo.pApplicationName = "Vulkan Microbenchmark";

    // Info about instance
    VkInstanceCreateInfo instanceInfo{};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.enabledLayerCount = 0;
    instanceInfo.ppEnabledLayerNames = nullptr;
    instanceInfo.enabledExtensionCount = 0;
    instanceInfo.ppEnabledExtensionNames = nullptr;
    instanceInfo.pApplicationInfo = &appInfo;

    // Check if instance creation fails
    if (vkCreateInstance(&instanceInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("Vulkan instance creation failed");
    }
}

void VkInfrastructure::createCmdPool() {

    // Create information about command pool
    VkCommandPoolCreateInfo cmdPoolInfo{};
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    cmdPoolInfo.queueFamilyIndex = computeQueueFamilyIndex;

    // Check if creation of command pool is successful
    if (vkCreateCommandPool(dev, &cmdPoolInfo, nullptr, &cmdPool) != VK_SUCCESS) {
        throw std::runtime_error("Creation of command pool failed");
    }
}