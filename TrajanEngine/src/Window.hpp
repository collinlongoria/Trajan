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

#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <TrajanEngine.hpp>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

class TRAJANENGINE_API Window {
public:
    Window(uint32_t width, uint32_t height, const std::string& name);
    ~Window() = default;

    // Poll and process GLFW events
    void PollEvents();

    // Returns if window should close
    bool ShouldClose() const;

    // Provides access to underlying GLFWwindow pointer
   [[nodiscard]] GLFWwindow* NativeWindow() const {
        return mWindow.get();
   }

    // Getters for window proeprties
    [[nodiscard]] uint32_t Width() const { return mWidth; }
    [[nodiscard]] uint32_t Height() const { return mHeight; }
    [[nodiscard]] const std::string& Name() const { return mName; }

    // Set key/input callback for the window
    void SetKeyCallback(GLFWkeyfun callback) {
       if(mWindow) {
           glfwSetKeyCallback(mWindow.get(), callback);
       }
   }

private:
    uint32_t mWidth, mHeight;
    std::string mName;
    std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)> mWindow;

    static bool sGLFWInitialized;
};



#endif //WINDOW_HPP
