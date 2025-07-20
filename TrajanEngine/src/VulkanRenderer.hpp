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

    void Shutdown() override;

private:
    void createInstance();
    void createSurface(GLFWwindow* window);
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapChain(GLFWwindow* window);
    void createImageViews();
    void createGraphicsPipeline();

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
};



#endif //VULKANRENDERER_HPP
