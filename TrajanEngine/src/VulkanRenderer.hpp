/*
* File: VulkanRenderer
* Project: Trajan
* Author: colli
* Created on: 7/15/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#ifndef VULKANRENDERER_HPP
#define VULKANRENDERER_HPP

#include <IRenderer.hpp>

#include <vulkan/vulkan_raii.hpp>

class VulkanRenderer : public IRenderer {
public:
    VulkanRenderer() = default;
    ~VulkanRenderer() override = default;

    void Initialize() override;

    void WindowInitialization(GLFWwindow* window) override;

    void Render(float dt) override;

    void Shutdown() override;

private:
    void createInstance();
    void createSurface(GLFWwindow* window);
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapChain(GLFWwindow* window);
    void createImageViews();
    void createGraphicsPipeline();
    [[nodiscard]] vk::raii::ShaderModule createShaderModule(const std::vector<char>& code) const;
    void createCommandPool();
    void createCommandBuffer();

    void createSyncObjects();

    void transitionImageLayout(uint32_t currentFrame, vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
        vk::AccessFlags2 srcAccessMask, vk::AccessFlags2 dstAccessMask, vk::PipelineStageFlags2 srcStageMask,
        vk::PipelineStageFlags2 dstStageMask);
    void recordCommandBuffer(uint32_t imageIndex);
    void drawFrame();

    std::vector<const char*>  getRequiredExtensions();

    /*
     * The order of declaration for vk::raii objects MUST
     * match the order that vulkan requires, or it will not work properly
     * and it will segfault upon shutdown.
     */
    // TODO: create a struct for window-specific objects to support multiple windows
    vk::raii::Context context;
    vk::raii::Instance instance                          = nullptr;
    vk::raii::SurfaceKHR surface                         = nullptr;

    vk::raii::PhysicalDevice physicalDevice              = nullptr;
    vk::raii::Device device                              = nullptr;

    vk::raii::Queue graphicsQueue                        = nullptr;
    vk::raii::Queue presentQueue                         = nullptr;

    vk::raii::SwapchainKHR swapChain                     = nullptr;
    std::vector<vk::raii::ImageView> swapChainImageViews;
    std::vector<vk::Image> swapChainImages;
    vk::Format swapChainImageFormat                      = vk::Format::eUndefined;
    vk::Extent2D swapChainExtent                         = {};

    vk::raii::PipelineLayout pipelineLayout              = nullptr;
    vk::raii::Pipeline graphicsPipeline                  = nullptr;
    vk::raii::CommandPool commandPool                    = nullptr;
    vk::raii::CommandBuffer commandBuffer                = nullptr;
    uint32_t graphicsIndex                               = 0;

    vk::raii::Semaphore presentCompleteSemaphore         = nullptr;
    vk::raii::Semaphore renderFinishedSemaphore          = nullptr;
    vk::raii::Fence drawFence                            = nullptr;
};



#endif //VULKANRENDERER_HPP
