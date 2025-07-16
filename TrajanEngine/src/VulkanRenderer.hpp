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

#include <vulkan/vulkan_raii.hpp>

class VulkanRenderer {
public:
    VulkanRenderer() = default;
    ~VulkanRenderer() = default;

    void initVulkan();

    void shutdownVulkan();

private:
    void createInstance();
    void pickPhysicalDevice();

    std::vector<const char*>  getRequiredExtensions();


    vk::raii::Context context;
    vk::raii::Instance instance = nullptr;

    vk::raii::PhysicalDevice physicalDevice = nullptr;
};



#endif //VULKANRENDERER_HPP
