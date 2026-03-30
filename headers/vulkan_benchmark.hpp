
#pragma once

#include "vulkan_infrastructure.hpp"
#include <vulkan/vulkan.h>
#include <string>
#include <vector>

class VkBenchmark {

private:

    // PRIVATE VARIABLES
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline pipeline = VK_NULL_HANDLE;
    VkShaderModule shaderModule = VK_NULL_HANDLE;
    VkFence fence = VK_NULL_HANDLE;
    VkCommandBuffer cmdBuffer = VK_NULL_HANDLE;

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
    void buildPipelineLayout();
    void buildPipeline();
    void buildShaderModule(const std::vector<char>& shaderCode);
    void buildFence();

    void setupCmdBuffer();
    void recordCmdBuffer();
    void submitCmdBuffer();

    void cleanup();

    std::vector<char> rdFile(const std::string& shaderFile) const;
};