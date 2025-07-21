/*
* File: IRenderer
* Project: Trajan
* Author: colli
* Created on: 7/17/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#ifndef IRENDERER_HPP
#define IRENDERER_HPP

#define GLFW_INCLUDE_VULKAN // TODO: something about this.
#include "GLFW/glfw3.h"

class IRenderer {
public:
    virtual ~IRenderer() = default;

    virtual void Initialize() = 0;

    virtual void WindowInitialization(GLFWwindow* window) = 0;

    virtual void Render(float dt) = 0;

    virtual void Shutdown() = 0;
};

#endif //IRENDERER_HPP
