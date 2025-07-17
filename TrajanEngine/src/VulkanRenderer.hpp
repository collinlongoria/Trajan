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


    /* Instance */
    vk::raii::Context context;
    vk::raii::Instance instance = nullptr;
    vk::raii::SurfaceKHR surface = nullptr;

    /* Devices */
    vk::raii::PhysicalDevice physicalDevice = nullptr; // Physical Device, i.e the GPU
    vk::raii::Device device = nullptr; // Logical Device, i.e the.... ???

    /* Queues */
    vk::raii::Queue graphicsQueue = nullptr;
    vk::raii::Queue presentQueue = nullptr;

    /* Swap Chain */
    vk::raii::SwapchainKHR swapChain = nullptr;
    std::vector<vk::Image> swapChainImages;
    vk::Format swapChainImageFormat = vk::Format::eUndefined;
    vk::Extent2D swapChainExtent = {};
    std::vector<vk::raii::ImageView> swapChainImageViews;

};



#endif //VULKANRENDERER_HPP
