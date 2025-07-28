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

#include <string>
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "i_renderer.hpp"

class Window {
public:
    Window(uint32_t width, uint32_t height, const std::string& name, RenderAPI api);
    ~Window();

    // Poll and process GLFW events
    void PollEvents();

    // Returns if window should close
    bool ShouldClose() const;

    // Provides access to underlying GLFWwindow pointer
   [[nodiscard]] GLFWwindow* NativeWindow() const {
        return mWindow;
   }

    // Getters for window proeprties
    [[nodiscard]] uint32_t Width() const { return mWidth; }
    [[nodiscard]] uint32_t Height() const { return mHeight; }
    [[nodiscard]] const std::string& Name() const { return mName; }

    // Set key/input callback for the window
    void SetKeyCallback(GLFWkeyfun callback) {
       if(mWindow) {
           glfwSetKeyCallback(mWindow, callback);
       }
   }

private:
    uint32_t mWidth, mHeight;
    std::string mName;
    GLFWwindow* mWindow;
};



#endif //WINDOW_HPP
