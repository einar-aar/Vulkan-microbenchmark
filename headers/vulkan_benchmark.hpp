
#pragma once

#include "vulkan_infrastructure.hpp"
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

class VkBenchmark {

private:

    // PRIVATE VARIABLES
    // These are needed to set up the pipeline with communication
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline pipeline = VK_NULL_HANDLE;
    VkShaderModule shaderModule = VK_NULL_HANDLE;
    VkFence fence = VK_NULL_HANDLE;
    VkCommandBuffer cmdBuffer = VK_NULL_HANDLE;
    VkQueryPool queryPool = VK_NULL_HANDLE;

    // These are needed to do real work on the GPU
    VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    VkDescriptorSetLayout descriptorLayout = VK_NULL_HANDLE;
    VkDeviceMemory bufferMem = VK_NULL_HANDLE;
    VkBuffer buffer = VK_NULL_HANDLE;

    VkInfrastructure& infrastructure;

public:

    // PUBLIC FUNCTIONS
    explicit VkBenchmark(VkInfrastructure& infrastructure);
    ~VkBenchmark();

    // DO NOT allow copying, as this leads to undefined behaviour when you have multiple objects
    // owning the same handles. (potential double destruction)
    VkBenchmark& operator=(const VkBenchmark&) = delete;
    VkBenchmark(const VkBenchmark&) = delete;

    void runBenchmark();

private:

    // PRIVATE FUNCTIONS
    // Memory
    void buildBuffer();
    void allocBufferMem();
    uint32_t findMemType(uint32_t filter, VkMemoryPropertyFlags propertyFlags) const;

    // Descriptor set
    void allocDescriptorSet();
    void buildDescriptorSetLayout();
    void buildDescriptorPool();
    void updateDescriptorSet();

    // Framework
    void buildPipelineLayout();
    void buildPipeline();
    void buildShaderModule(const std::vector<char>& shaderCode);
    
    // Communication
    void buildFence();
    void buildQueryPool();
    void setupCmdBuffer();
    void recordCmdBuffer();
    void submitCmdBuffer();

    void cleanup();

    void printGpuTime();
    std::vector<char> rdFile(const std::string& shaderFile) const;
};