
#pragma once

#include <vulkan/vulkan.h> // Including the Vulkan SDK includes
#include <optional>
#include <vector>

class VkInfrastructure {

private:

    // PRIVATE VARIABLES
    VkPhysicalDevice physicalDev = VK_NULL_HANDLE;
    VkDevice dev = VK_NULL_HANDLE;
    VkInstance instance = VK_NULL_HANDLE;
    VkCommandPool cmdPool = VK_NULL_HANDLE;
    VkQueue queue = VK_NULL_HANDLE;
    uint32_t computeQueueFamilyIndex = 0;

    // Struct to save temporary family index for relevant queues
    struct QueueFamilyIndex {
        std::optional<uint32_t> familyIndex;
        bool hasValue() const { return familyIndex.has_value(); }
    };

public:
    
    // PUBLIC FUNCTIONS
    explicit VkInfrastructure();
    ~VkInfrastructure();

    // DO NOT allow copying, as this leads to undefined behaviour when you have multiple objects
    // owning the same handles. (potential double destruction)
    VkInfrastructure& operator= (const VkInfrastructure&) = delete;
    VkInfrastructure(const VkInfrastructure&) = delete;

    // Getter functions
    VkPhysicalDevice getPhysicalDev() const { return physicalDev; }
    VkDevice getDev() const { return dev; }
    VkInstance getVKInstance() const { return instance; }
    VkCommandPool getCmdPool() const { return cmdPool; }
    VkQueue getQueue() const { return queue; }
    uint32_t getComputeQueueFamilyIndex() const { return computeQueueFamilyIndex; }

private:

    // PRIVATE FUNCTIONS
    // Choosing physical and creating logical device
    void choosePhysicalDev();
    QueueFamilyIndex getQueueFamilies(VkPhysicalDevice dev) const;
    bool checkDevSuitability(VkPhysicalDevice dev) const;
    void createLogicalDev();

    // Setting up other infrastructure
    void createInstance();
    void createCmdPool();
};