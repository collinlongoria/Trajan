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
#include <variant>

#include "math.hpp"

struct ImGuiContext;

// ------------ Render API Choice ------------
enum class RenderAPI {
    OpenGL,
    Vulkan,
};

// ------------ Initialization Info ------------
struct RendererInitInfo {
    void* nativeWindowHandle = nullptr;
    uint32_t width = 0;
    uint32_t height = 0;
    RenderAPI preferredAPI = RenderAPI::OpenGL;
};

// ------------ Forward declarations ------------
class Mesh;
class Texture;
class Shader;

// ------------ Vertex Layout Description ------------
enum class VertexSemantic {
    Position,
    Normal,
    Tangent,
    Bitangent,
    Color0,
    TexCoord0,
    TexCoord1,
    // TODO: Add more as needed
    Custom
};

// I do not know what will really be used. So I added a bunch
enum class VertexDataType {
    Float32,
    UInt8,
    UInt16,
    UInt32,
    Int8,
    Int16,
    Int32,
    UNorm8,
    UNorm16,
    HalfFloat
};

struct VertexAttrib {
    VertexSemantic semantic = VertexSemantic::Position;
    VertexDataType type = VertexDataType::Float32;
    uint8_t componentCount = 3; // 1..4
    bool normalized = false;    // <- For UNorm/Int types
    uint32_t offset = 0;        // Byte offset from vertex start
};

struct VertexLayoutDesc {
    uint32_t stride = 0; // If 0 -> Computed from max(offset) + size
    std::vector<VertexAttrib> attribs;
};

// ------------ Resource Descriptors ------------
struct MeshDescriptor {
    const void* vertexData = nullptr;
    size_t vertexSize = 0;
    const void* indexData = nullptr;
    size_t indexSize = 0;
    VertexLayoutDesc layout;
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

// ------------ Per-Draw Uniforms ------------
using UniformValue = std::variant<int, float, Matrix4>; // TODO: Add rest of possible variable types

struct UniformAssignment {
    std::string name;
    UniformValue value;
};

// ------------ Frame Data (Consistent UBO) ------------
struct FrameData {
    Matrix4 view = Matrix4(1.0f);
    Matrix4 proj = Matrix4(1.0f);
    Vector3 cameraPos = Vector3(0.0f);
};

// ------------ Abstract Render Command ------------
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

    std::vector<UniformAssignment> uniforms;
    std::function<void()> callback;
};

class IRenderer {
public:
    virtual ~IRenderer() = default;

    // Initialization
    virtual void Initialize(const RendererInitInfo& initInfo) = 0;
    virtual void Resize(uint32_t width, uint32_t height) = 0;

    // Frame Lifecycle
    virtual void SetFrameData(const FrameData& fd) = 0;
    virtual void BeginFrame() = 0;
    virtual void SubmitRenderCommand(const RenderCommand& cmd) = 0;
    virtual void EndFrame() = 0;

    // imgui Context
    [[nodiscard]] virtual ImGuiContext* GetImGuiContext() const = 0;

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
