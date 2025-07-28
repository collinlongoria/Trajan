/*
* File: Window
* Project: Trajan
* Author: colli
* Created on: 7/15/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#include "window.hpp"

#include "log.hpp"

Window::Window(uint32_t width, uint32_t height, const std::string &name, RenderAPI api)
    : mWidth(width), mHeight(height), mName(name), mWindow(nullptr)
{
    static bool sGLFWInitialized = false;
    if(!sGLFWInitialized) {
        if(!glfwInit()) {
            Log::Assert(false, "GLFW initialization failed!");
        }
        sGLFWInitialized = true;
    }

    if( api == RenderAPI::Vulkan ) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }
    else if ( api == RenderAPI::OpenGL ) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }

    // Create GLFW window
    Log::Message("Creating window...");
    GLFWwindow* rawWindow = (glfwCreateWindow(mWidth, mHeight, mName.c_str(), nullptr, nullptr));
    if(!rawWindow) {
        Log::Error("Failed to create GLFW window!");
        return;
    }

    // If renderer was passed, assume extra work needed


    mWindow = rawWindow;

    Log::Message("Window created!");
}

Window::~Window() {
    if( mWindow ) {
        Log::Message("Window " + mName + " destroyed.");
        glfwDestroyWindow(mWindow);
    }

    // TODO: add static ref count and only terminate on last window being destroyed
    glfwTerminate();
}

void Window::PollEvents() {
    glfwPollEvents();
}

bool Window::ShouldClose() const {
    return mWindow ? glfwWindowShouldClose(mWindow) : true;
}

