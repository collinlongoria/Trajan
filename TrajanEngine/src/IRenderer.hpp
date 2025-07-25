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

/*
#define GLFW_INCLUDE_VULKAN // TODO: something about this.
#include "GLFW/glfw3.h"
*/

#include <string>
#include "Math.hpp"

enum class RenderAPI {
    OpenGL,
    Vulkan,
};

struct RendererInitInfo {
    void* nativeWindowHandle = nullptr;
    uint32_t width = 0;
    uint32_t height = 0;
    RenderAPI preferredAPI = RenderAPI::OpenGL;
};

// Forward declarations
class Mesh;
class Texture;
class Shader;

// Abstract Render Command
struct RenderCommand {
    enum class Type {
        Mesh,
        CustomCallback
    };

    Type type = Type::Mesh;

    Matrix4 transform = Matrix4(1.0f);
    const Mesh* mesh = nullptr;
    const Shader* shader = nullptr;
    const Texture* texture = nullptr;

    std::function<void()> callback;
};

// Resource Descriptors
struct MeshDescriptor {
    const void* vertexData = nullptr;
    size_t vertexSize = 0;
    const void* indexData = nullptr;
    size_t indexSize = 0;
};

struct TextureDescriptor {
    const void* pixelData = nullptr;
    uint32_t width = 0;
    uint32_t height = 0;
    bool generateMipmaps = true;
    bool sRGB = false;
};

struct ShaderDescriptor {
    // TODO: get this working, but need to support other kinds of shaders and better align with ShaderCompiler
    std::string vertexSource;
    std::string fragmentSource;
};

class IRenderer {
public:
    virtual ~IRenderer() = default;

    // Initialization
    virtual void Initialize(const RendererInitInfo& initInfo) = 0;
    virtual void Resize(uint32_t width, uint32_t height) = 0;

    // Frame Lifecycle
    virtual void BeginFrame() = 0;
    virtual void SubmitRenderCommand(const RenderCommand& cmd) = 0;
    virtual void EndFrame() = 0;

    // TODO: Consider GUI render function handler (for imgui...)
    // virtual void RenderImGui() {} ???

    // Resource Factory Methods
    virtual uint64_t CreateMesh(const MeshDescriptor& desc) = 0;
    virtual uint64_t CreateTexture(const TextureDescriptor& desc) = 0;
    virtual uint64_t CreateShader(const ShaderDescriptor& desc) = 0;

    // Destruction Methods
    virtual void DestroyMesh(uint64_t handle) = 0;
    virtual void DestroyTexture(uint64_t handle) = 0;
    virtual void DestroyShader(uint64_t handle) = 0;

    // Cleanup
    virtual void Cleanup() = 0;
};

#endif //IRENDERER_HPP
