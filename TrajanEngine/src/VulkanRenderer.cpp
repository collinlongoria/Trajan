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

#include <iostream>
#include <limits>

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


void VulkanRenderer::Initialize() {
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

void VulkanRenderer::WindowInitialization(GLFWwindow* window) {
    if(instance == nullptr) {
        Log::Error("Vulkan renderer has not been initialized yet, cannot create window surface!");
        return;
    }

    createSurface(window);
    // TODO: don't like that these have to be done in window creation function. consider options.
    createLogicalDevice(); // required after window creation
    createSwapChain(window); // required after window creation <-- unique per window
    createImageViews(); // required after window creation <-- unique per window
    createGraphicsPipeline();
}


void VulkanRenderer::Shutdown() {
    // Good manners
    if( device != nullptr ) device.waitIdle();

    /*
    // clear window related variables
    swapChainImageViews.clear();
    swapChainImages.clear();
    swapChain = nullptr;

    // destory queues
    graphicsQueue = nullptr;
    presentQueue = nullptr;

    // destroy vulkan objects
    device = nullptr;
    surface = nullptr;
    instance = nullptr;
    context = vk::raii::Context(); // resets the context?
*/
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
    context = vk::raii::Context();

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

void VulkanRenderer::createLogicalDevice() {
    // Retrieve index of the first queue family that supports graphics
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

    // Retrieve the first index into queueFamilyProperties which supports graphics
    auto graphicsQueueFamilyProperty = std::ranges::find_if( queueFamilyProperties, []( auto const & qfp ) {
        return (qfp.queueFlags & vk::QueueFlagBits::eGraphics ) != static_cast<vk::QueueFlags>(0);
    });
    Log::Assert(graphicsQueueFamilyProperty != queueFamilyProperties.end(), "No graphics supporting queue family found!");

    auto graphicsIndex = static_cast<uint32_t>( std::distance( queueFamilyProperties.begin(), graphicsQueueFamilyProperty ) );

    // Determine a queueFamilyIndex that supports present
    // Check if the graphicsIndex is good enoguh
    auto presentIndex = physicalDevice.getSurfaceSupportKHR( graphicsIndex, *surface )
        ? graphicsIndex
        : ~0;
    if( presentIndex == queueFamilyProperties.size() ) {
        // The graphicsIndex doesn't support present -> look for another family that supports both graphics & present
        for( size_t i = 0; i < queueFamilyProperties.size(); i++ ) {
            if( ( queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics ) &&
                physicalDevice.getSurfaceSupportKHR( static_cast<uint32_t>( i ), *surface ) ) {
                graphicsIndex = static_cast<uint32_t>( i );
                presentIndex = graphicsIndex;
                break;
            }
        }
        if( presentIndex == queueFamilyProperties.size() ) {
            // There is no single family index that supports both -> look for at least one that supports present
            for( size_t i = 0; i < queueFamilyProperties.size(); i++ ) {
                if( physicalDevice.getSurfaceSupportKHR( static_cast<uint32_t>( i ), *surface ) ) {
                    presentIndex = static_cast<uint32_t>( i );
                    break;
                }
            }
        }
    }
    if( ( graphicsIndex == queueFamilyProperties.size() ) && ( presentIndex == queueFamilyProperties.size() ) ) {
        Log::Assert(false, "Could not find a queue for graphics or present. Terminating.");
    }

    // Query for Vulkan 1.4 features
    vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan14Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain = {
        { }, // vk::PhysicalDeviceFeatures2
        { /*.dynamicRendering = true*/ }, // vk::PhysicalDeviceVulkan14Features
        { .extendedDynamicState = true } // vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
    };

    // Create a device
    float queuePriority = 0.f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
        .queueFamilyIndex = graphicsIndex,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority
    };
    vk::DeviceCreateInfo deviceCreateInfo{
        .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos  = &deviceQueueCreateInfo,
        .enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtension.size()),
        .ppEnabledExtensionNames = requiredDeviceExtension.data()
    };

    Log::Message("Creating logical device...");
    device = vk::raii::Device( physicalDevice, deviceCreateInfo );

    Log::Message("Creating graphics queue...");
    graphicsQueue = vk::raii::Queue( device, graphicsIndex, 0 );

    Log::Message("Creating present queue...");
    presentQueue = vk::raii::Queue( device, graphicsIndex, 0 );
}

void VulkanRenderer::createSurface(GLFWwindow* window) {
    VkSurfaceKHR _surface;
    if(glfwCreateWindowSurface(*instance, window, nullptr, &_surface) != 0) {
        Log::Assert(false, "failed to create window surface!");
    }
    surface = vk::raii::SurfaceKHR(instance, _surface);
}

static vk::Format chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
    const auto formatIt = std::ranges::find_if( availableFormats, []( auto const & format ) {
        return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
    });
    return formatIt != availableFormats.end() ? formatIt->format : availableFormats.front().format;
}

static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) {
    return std::ranges::any_of( availablePresentModes, [](const vk::PresentModeKHR value ) {
        return vk::PresentModeKHR::eMailbox == value; } ) ? vk::PresentModeKHR::eMailbox : vk::PresentModeKHR::eFifo;
}

vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {
    if( capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() ) {
        return capabilities.currentExtent;
    }
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    return{
        std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
        std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
    };
}

void VulkanRenderer::createSwapChain(GLFWwindow* window) {
    // Query for basic surface capabilities
    auto surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR( surface );
    // Choose Image Format and Extents
    swapChainImageFormat = chooseSwapSurfaceFormat( physicalDevice.getSurfaceFormatsKHR( surface ) );
    swapChainExtent = chooseSwapExtent( surfaceCapabilities, window );
    auto minImageCount = std::max( 3u, surfaceCapabilities.minImageCount );
    minImageCount = ( surfaceCapabilities.maxImageCount > 0 && minImageCount > surfaceCapabilities.maxImageCount ) ? surfaceCapabilities.maxImageCount : minImageCount;
    vk::SwapchainCreateInfoKHR swapChainCreateInfo{
        .surface = surface,
        .minImageCount = minImageCount,
        .imageFormat = swapChainImageFormat,
        .imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear,
        .imageExtent = swapChainExtent,
        .imageArrayLayers = 1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = vk::SharingMode::eExclusive,
        .preTransform = surfaceCapabilities.currentTransform,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode = chooseSwapPresentMode(physicalDevice.getSurfacePresentModesKHR( surface )),
        .clipped = true
    };

    Log::Message("Creating swap chain...");
    swapChain = vk::raii::SwapchainKHR( device, swapChainCreateInfo );
    swapChainImages = swapChain.getImages();
}

void VulkanRenderer::createImageViews() {
    swapChainImageViews.clear();

    vk::ImageViewCreateInfo imageViewCreateInfo{
        .viewType = vk::ImageViewType::e2D,
        .format = swapChainImageFormat,
        .subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }
    };
    Log::Message("Creating image views...");
    for( auto image : swapChainImages ) {
        imageViewCreateInfo.image = image;
        swapChainImageViews.emplace_back( device, imageViewCreateInfo );
    }
}

void VulkanRenderer::createGraphicsPipeline() {

}

