
#include "vulkan_benchmark.hpp"

#include <cstdint>
#include <fstream>
#include <stdexcept>

VkBenchmark::VkBenchmark(VkInfrastructure& infrastructure) : infrastructure(infrastructure) {}

VkBenchmark::~VkBenchmark() { cleanup() };

void VkBenchmark::runBenchmark() {

    const std::vector<char> shaderCode = rdFile("shaders/compute_shader.comp.spv");

    // The sequence of functions matter!
    buildShaderModule(shaderCode);
    buildPipelineLayout();
    buildPipeline();
    setupCmdBuffer();
    recordCmdBuffer();
    buildFence();
    submitCmdBuffer();
}

void VkBenchmark::buildPipelineLayout() {

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{ .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                                                   .pushConstantRangeCount = 0,
                                                   .pPushConstantRanges = nullptr,
                                                   .setLayoutCount = 0,
                                                   .pSetLayouts = nullptr };

    if (vkCreatePipelineLayout(infrastructure.getDev(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("Creation of pipeline layout failed");
    }
}

void VkBenchmark::buildPipeline() {

    VkPipelineShaderStageCreateInfo shaderStageInfo{ .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                                                     .module = shaderModule,
                                                     .pName = "main",
                                                     .stage = VK_SHADER_STAGE_COMPUTE_BIT };

    VkComputePipelineCreateInfo pipelineInfo{ .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
                                              .basePipelineIndex = -1,
                                              .basePipelineHandle = VK_NULL_HANDLE,
                                              .layout = pipelineLayout,
                                              .stage = shaderStageInfo };

    if (vkCreateComputePipelines(infrastructure.getDev(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
        throw std::runtime_error("Creation of computation pipeline failed");
    }
}

void VkBenchmark::buildShaderModule(const std::vector<char>& shaderCode) {

    VkShaderModuleCreateInfo shaderModuleInfo{ .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                                               // .pCode expects 32 bit unsigned pointer, shaderCode is char (8 bit on most systems)
                                               .pCode = reinterpret_cast<const uint32_t*>(shaderCode.data()),
                                               .codeSize = shaderCode.size() };

    if (vkCreateShaderModule(infrastructure.getDev(), &shaderModuleInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("Creation of shader module failed");
    }
}

void VkBenchmark::buildFence() {

    VkFenceCreateInfo fenceInfo{ .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                                 .flags = 0 };

    if (vkCreateFence(infrastructure.getDev(), &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
        throw std::runtime_error("Creation of fence failed");
    }
}

void VkBenchmark::setupCmdBuffer() {

    VkCommandBufferAllocateInfo cmdBufferSetupInfo{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                                            .commandBufferCount = 1,
                                                            .commandPool = infrastructure.getCmdPool(),
                                                            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY };
                                                        
    if (vkAllocateCommandBuffers(infrastructure.getDev(), &cmdBufferSetupInfo, &cmdBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Setup of command buffer failed");
    }
}

void VkBenchmark::recordCmdBuffer() {

    VkCommandBufferBeginInfo cmdBufferRecordingInfo { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                                          .pInheritanceInfo = nullptr,
                                                          .flags = 0 };

    if (vkBeginCommandBuffer(cmdBuffer, &cmdBufferRecordingInfo) != VK_SUCCESS) {
        throw std::runtime_error("Starting recording of command buffer failed");
    }

    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
    vkCmdDispatch(cmdBuffer, 1, 1, 1);

    if (vkEndCommandBuffer(cmdBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Ending of command buffer recording failed");
    }
}

void VkBenchmark::submitCmdBuffer() {

    VkSubmitInfo submitCmdBufferInfo{ .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                      .pCommandBuffers = &cmdBuffer,
                                      .commandBufferCount = 1 };

    if (vkQueueSubmit(infrastructure.getQueue(), 1, &submitCmdBufferInfo, fence) != VK_SUCCESS) {
        throw std::runtime_error("Submitting of command buffer failed");
    }

    if (vkWaitForFences(infrastructure.getDev(), 1, &fence, VK_TRUE, UINT64_MAX) != VK_SUCCESS) {
        throw std::runtime_error("Waiting for fence led to an error");
    }
}

void VkBenchmark::cleanup() {

    // Destroy if they still exist
    if (fence != VK_NULL_HANDLE) vkDestroyFence(infrastructure.getDev(), fence, nullptr);
    if (cmdBuffer != VK_NULL_HANDLE) vkFreeCommandBuffers(infrastructure.getDev(), infrastructure.getCmdPool(), 1, &cmdBuffer);
    if (pipeline != VK_NULL_HANDLE) vkDestroyPipeline(infrastructure.getDev(), pipeline, nullptr);
    if (pipelineLayout != VK_NULL_HANDLE) vkDestroyPipelineLayout(infrastructure.getDev(), pipelineLayout, nullptr);
    if (shaderModule != VK_NULL_HANDLE) vkDestroyShaderModule(infrastructure.getDev(), shaderModule, nullptr);
}

std::vector<char> VkBenchmark::rdFile(const std::string& shaderFile) const {

    // Opening at the end of the file to retrieve file size, reading in binary mode
    std::ifstream file(shaderFile, std::ios::ate | std::ios::binary);

    if (!file.is_open()) throw std::runtime_error("Could not open shader file");

    // Get file size from checking position at end of file
    const std::streamsize fileSize = file.tellg();

    std::vector<char> shaderCodeBuffer(static_cast<size_t>(fileSize));

    // Move to the start and read the whole file before closing it.
    file.seekg(0);
    file.read(shaderCodeBuffer.data(), fileSize);
    file.close();

    if (file.is_open()) throw std::runtime_error("Failed to close shader file");

    return shaderCodeBuffer;
}
