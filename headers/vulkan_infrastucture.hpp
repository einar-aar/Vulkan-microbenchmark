
#pragma once

#include <vulkan/vulkan.h> // Including the Vulkan SDK includes
#include <optional>
#include <vector>

class VKInfrastructure {

private:

    // PRIVATE VARIABLES
    VkPhysicalDevice physicalDev = VK_NULL_HANDLE;
    VkDevice dev = VK_NULL_HANDLE;
    VkInstance instance = VK_NULL_HANDLE;
    VkCommandPool cmdPool = VK_NULL_HANDLE;
    VkQueue queue = VK_NULL_HANDLE;
    uint32_t queueFamilyIndex = 0;

public:
    
    // PUBLIC FUNCTIONS
    VKInfrastructure();
    ~VKInfrastructure();

    // DO NOT allow copying, as this leads to undefined behaviour when you have multiple objects
    // owning the same handles. (potential double destruction)
    VKInfrastructure& operator= (const VKInfrastructure&) = delete;
    VKInfrastructure(const VKInfrastructure&) = delete;

    // Getter functions
    VkPhysicalDevice getPhysicalDev() const { return physicalDev; }
    VkDevice getDev() const { return dev; }
    VkInstance getVKInstance() const { return instance; }
    VkCommandPool getCmdPool() const { return cmdPool; }
    VkQueue getQueue() const { return queue; }
    uint32_t getQueueFamilyIndex() const { return queueFamilyIndex; }

private:

    // PRIVATE FUNCTIONS
    // Choosing physical and creating logical device
    void choosePhysicalDev();
    QueueFamilyTempIndex getQueueFamilies(VkPhysicalDevice dev) const;
    bool checkDevSuitability(VkPhysicalDevice dev) const;
    void createLogicalDev();

    // Setting up other infrastructure
    void createInstance();
    void createCmdPool();

    struct QueueFamilyTempIndex {
        std::optional<uint32_t> family;
        bool hasValue() const { return family.has_value(); }
    };
};