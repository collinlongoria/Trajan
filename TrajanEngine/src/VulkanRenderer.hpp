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

    /*
    void Initialize() override;

    void WindowInitialization(GLFWwindow* window) override;

    void Render(float dt) override;

    void Shutdown() override;
*/
    bool framebufferResized = false;
private:
    /*
    void createInstance();
    void createSurface(GLFWwindow* window);
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapChain();
    void createImageViews();
    void createGraphicsPipeline();
    [[nodiscard]] vk::raii::ShaderModule createShaderModule(const std::vector<char>& code) const;
    void createCommandPool();
    void createCommandBuffers();

    void createSyncObjects();

    void transitionImageLayout(uint32_t imageIndex, vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
        vk::AccessFlags2 srcAccessMask, vk::AccessFlags2 dstAccessMask, vk::PipelineStageFlags2 srcStageMask,
        vk::PipelineStageFlags2 dstStageMask);
    void recordCommandBuffer(uint32_t imageIndex);
    void drawFrame();

    void cleanupSwapChain();
    void recreateSwapChain();

    std::vector<const char*>  getRequiredExtensions();

    GLFWwindow* registeredWindow;

    const int MAX_FRAMES_IN_FLIGHT = 2;
    uint32_t currentFrame   = 0;
    uint32_t semaphoreIndex = 0;
    /*
     * The order of declaration for vk::raii objects MUST
     * match the order that vulkan requires, or it will not work properly
     * and it will segfault upon shutdown.

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
    std::vector<vk::raii::CommandBuffer> commandBuffers;
    uint32_t graphicsIndex                               = 0;

    std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
    std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
    std::vector<vk::raii::Fence> inFlightFences;
    */
};



#endif //VULKANRENDERER_HPP
