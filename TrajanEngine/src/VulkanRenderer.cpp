/*
* File: VulkanRenderer
* Project: Trajan
* Author: Collin Longoria
* Created on: 7/15/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/
#include "VulkanRenderer.hpp"
#include <vulkan/vk_platform.h>

#include <vector>

#include "Log.hpp"
#include "GLFW/glfw3.h"

// Validation layer
const std::vector validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
    constexpr bool enableValidationLayers = false;
#else
    constexpr bool enableValidationLayers = true;
#endif

// Debug messenger callback function
static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
    vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*) {
    if(severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eError || severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
        Log::Error("validation layer: type " + to_string(type) + " msg: " + pCallbackData->pMessage);
    }

    return vk::False;
}

void VulkanRenderer::initVulkan() {
    createInstance();
    setupDebugMessenger();
}

void VulkanRenderer::drawLoop() {

}

// Helper function to get required extensions for Vulkan
std::vector<const char*> VulkanRenderer::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    // Get GLFW required extensions
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    // Must add debug utils if using validation layers
    if(enableValidationLayers) {
        extensions.push_back(vk::EXTDebugUtilsExtensionName);
    }

    return extensions;
}

// Helper function to setup the debug messenger
void VulkanRenderer::setupDebugMessenger() {
    if(!enableValidationLayers) return;

    vk::DebugUtilsMessageSeverityFlagsEXT severityFlags( vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
    vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags( vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation );

    vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT{
        .messageSeverity = severityFlags,
        .messageType = messageTypeFlags,
        .pfnUserCallback = &debugCallback
    };

    debugMessenger = instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
}

void VulkanRenderer::createInstance() {
    /*
     * VkApplicationInfo provides optional metadata about the application
     * which can help drivers optimize or track usage
     */
    constexpr vk::ApplicationInfo appInfo{ .pApplicationName = "Trajan",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "Trajan Engine",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = vk::ApiVersion14
    };

    // Get required validation layers
    std::vector<char const*> requiredLayers;
    if(enableValidationLayers) {
        requiredLayers.assign(validationLayers.begin(), validationLayers.end());
    }

    // Check if the required layers are supported by the Vulkan implementation
    auto layerProperties = context.enumerateInstanceLayerProperties();
    if (std::ranges::any_of(requiredLayers, [&layerProperties](auto const& requiredLayer) {
        return std::ranges::none_of(layerProperties,
                                   [requiredLayer](auto const& layerProperty)
                                   { return strcmp(layerProperty.layerName, requiredLayer) == 0; });
    }))
    {
        Log::Assert(false, "One or more required layers are not supported!");
    }

    // Add required extensions
    auto extensions = getRequiredExtensions();

    /*
     * VkInstanceCreateInfo defines parameters for creating a VkInstance,
     * including which extensions and validation layers to enable, and
     * a pointer to VkApplicationInfo
     */
    vk::InstanceCreateInfo createInfo{
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<uint32_t>(validationLayers.size()),
        .ppEnabledLayerNames = requiredLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data()
    };

    instance = vk::raii::Instance(context, createInfo);
    Log::Message("Vulkan Instance created");
}



