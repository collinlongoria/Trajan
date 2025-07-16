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

Window::Window(uint32_t width, uint32_t height, const std::string &name)
    : mWidth(width), mHeight(height), mName(name), mWindow(nullptr)
{
    // Create GLFW window
    Log::Message("Creating window...");
    GLFWwindow* rawWindow = (glfwCreateWindow(mWidth, mHeight, mName.c_str(), nullptr, nullptr));
    if(!rawWindow) {
        Log::Error("Failed to create GLFW window!");
        return;
    }

    mWindow = rawWindow;

    Log::Message("Window created!");
}

Window::~Window() {
    Log::Message("Window " + mName + "  destroyed.");
    glfwDestroyWindow(mWindow);
}

void Window::PollEvents() {
    glfwPollEvents();
}

bool Window::ShouldClose() const {
    return mWindow ? glfwWindowShouldClose(mWindow) : true;
}

