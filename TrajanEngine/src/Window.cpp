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

#include "Window.hpp"

#include "Log.hpp"

bool Window::sGLFWInitialized = false;

Window::Window(uint32_t width, uint32_t height, const std::string &name)
    : mWidth(width), mHeight(height), mName(name), mWindow(nullptr, glfwDestroyWindow)
{
    // Init GLFW
    if(!sGLFWInitialized) {
        Log::Message("Initializing GLFW...");
        if(!glfwInit()) {
            Log::Error("Failed to initialize GLFW!");
            return;
        }
        sGLFWInitialized = true;
    }

    // Set GLFW window hints
    // TODO: Move these as needed
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // vulkan
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    //glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);

    // Create GLFW window
    Log::Message("Creating window...");
    GLFWwindow* rawWindow = (glfwCreateWindow(mWidth, mHeight, mName.c_str(), nullptr, nullptr));
    if(!rawWindow) {
        Log::Error("Failed to create GLFW window!");
        return;
    }

    mWindow.reset(rawWindow);

    Log::Message("Window created!");
}

void Window::PollEvents() {
    glfwPollEvents();
}

bool Window::ShouldClose() const {
    return mWindow ? glfwWindowShouldClose(mWindow.get()) : true;
}

