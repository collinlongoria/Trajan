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
#include <map>

#include "Log.hpp"

#define GLFW_INCLUDE_VULKAN
#include <iostream>

#include "GLFW/glfw3.h"

// Required Vulkan extensions
std::vector<const char*> requiredDeviceExtension = {
    vk::KHRSwapchainExtensionName,
    vk::KHRSpirv14ExtensionName,
    vk::KHRSynchronization2ExtensionName,
    vk::KHRCreateRenderpass2ExtensionName
};

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
    if(severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eError) {
        Log::Error("validation layer: type " + to_string(type) + " msg: " + pCallbackData->pMessage);
    }
    else if(severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
        Log::Warn("validation layer: type " + to_string(type) + " msg: " + pCallbackData->pMessage);
    }

    return vk::False;
}


void VulkanRenderer::initVulkan() {
    // Initialize GLFW
    Log::Message("Initializing GLFW...");
    if(!glfwInit()) {
        Log::Error("Failed to initialize GLFW!");
        return;
    }
    // Set GLFW window hints
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // vulkan
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    //glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);

    // Initialize Vulkan objects

    createInstance();
    pickPhysicalDevice();
}

void VulkanRenderer::shutdownVulkan() {
    // destroy vulkan objects
    instance = nullptr;

    glfwTerminate();
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

void VulkanRenderer::createInstance() {
    // Start by creating the base context
    // (it was crashing before I did this)
    Log::Message("Creating Vulkan context...");
    //context = vk::raii::Context();

    vk::DebugUtilsMessengerCreateInfoEXT dbgCreateInfo{
        .messageSeverity =
              vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose
            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo
            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
            | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
        .messageType =
              vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
            | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
            | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
        .pfnUserCallback = debugCallback
    };

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
    auto availableLayers = context.enumerateInstanceLayerProperties();
    for(const char* layerName : requiredLayers) {
        bool found = std::ranges::any_of(availableLayers, [&](const auto& prop) {
            return strcmp(layerName, prop.layerName) == 0;
        });

        if(!found) {
            Log::Assert(false, std::string("Requested validation layer not available: ") + layerName);
        }
    }

    // Add required extensions
    auto requiredExtensions = getRequiredExtensions();

    // Check if required extensions are supported by Vulkan
    auto extensionProperties = context.enumerateInstanceExtensionProperties();
    for(auto const& requiredExtension : requiredExtensions) {
        if(std::ranges::none_of(extensionProperties, [requiredExtension](auto const& extensionProperty) { return strcmp(extensionProperty.extensionName, requiredExtension) == 0;}))
        {
            Log::Assert(false, "Required extension not supported " + std::string(requiredExtension));
        }
    }

    /*
     * VkInstanceCreateInfo defines parameters for creating a VkInstance,
     * including which extensions and validation layers to enable, and
     * a pointer to VkApplicationInfo
     */
    vk::InstanceCreateInfo createInfo{
        .pNext                   = enableValidationLayers ? &dbgCreateInfo : nullptr,
        .pApplicationInfo        = &appInfo,
        .enabledLayerCount       = enableValidationLayers ? static_cast<uint32_t>(requiredLayers.size()) : 0,
        .ppEnabledLayerNames     = enableValidationLayers ? requiredLayers.data() : nullptr,
        .enabledExtensionCount   = static_cast<uint32_t>(requiredExtensions.size()),
        .ppEnabledExtensionNames = requiredExtensions.data()
    };

    Log::Message("Creating Vulkan instance...");
    instance = vk::raii::Instance(context, createInfo);
}

void VulkanRenderer::pickPhysicalDevice() {
    // Get list of available physical devices
    auto devices = vk::raii::PhysicalDevices( instance );

    // Iterate through to find first suitable GPU
    const auto devIter = std::ranges::find_if( devices,

    [&]( auto const & device ) {
            // Check if the device supports the Vulkan 1.4 API version
            bool supportsVulkan1_4 = device.getProperties().apiVersion >= VK_API_VERSION_1_4;

            // Check if any of the queue families support graphics operations
            auto queueFamilies = device.getQueueFamilyProperties();
            bool supportsGraphics = std::ranges::any_of( queueFamilies, []( auto const & qfp ) { return !!( qfp.queueFlags & vk::QueueFlagBits::eGraphics ); } );

            // Check if all required device extensions are available
            auto availableDeviceExtensions = device.enumerateDeviceExtensionProperties();
            bool supportsAllRequiredExtensions = std::ranges::all_of( requiredDeviceExtension,
                [&availableDeviceExtensions]( auto const & requiredDeviceExtension )
                {
                     return std::ranges::any_of( availableDeviceExtensions,
                        [requiredDeviceExtension]( auto const & availableDeviceExtension ) { return strcmp( availableDeviceExtension.extensionName, requiredDeviceExtension ) == 0; } );
                }
            );

        // this part seems to only be related to dynamic renders - but this became a core feature so not sure this is still needed.
        /*
            auto features  = device.template getFeatures2<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan14Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
            bool supportsRequiredFeatures = features.template get<vk::PhysicalDeviceVulkan14Features>().dynamicRendering &&
                                            features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;
        */
            return supportsVulkan1_4 && supportsGraphics && supportsAllRequiredExtensions;
        }
    );

    if ( devIter != devices.end() )
    {
        physicalDevice = vk::raii::PhysicalDevice(*devIter);
        Log::Message("Vulkan Physical Device found: " + std::string(physicalDevice.getProperties().deviceName));
    }
    else
    {
        Log::Assert(false, "failed to find a suitable GPU!" );
    }
}


