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
    void initVulkan();

    void drawLoop();

private:
    void createInstance();

    std::vector<const char*>  getRequiredExtensions();
    void setupDebugMessenger();


    vk::raii::Context context;
    vk::raii::Instance instance = nullptr;
    vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
};



#endif //VULKANRENDERER_HPP
