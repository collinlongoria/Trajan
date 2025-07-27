/*
* File: OpenGLRenderer.hpp
* Project: Trajan
* Author: ${AUTHOR}
* Created on: 7/24/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#ifndef OPENGLRENDERER_HPP
#define OPENGLRENDERER_HPP

#include "IRenderer.hpp"
#include <glad/glad.h>
#include <unordered_map>
#include <vector>

#include "Mesh.hpp"
#include "Shader.hpp"

class OpenGLRenderer : public IRenderer {
public:
    void Initialize(const RendererInitInfo &initInfo) override;
    void Resize(uint32_t width, uint32_t height) override;

    void SetFrameData(const FrameData &fd) override;

    void BeginFrame() override;
    void SubmitRenderCommand(const RenderCommand &cmd) override;
    void EndFrame() override;

    // Resource Management
    uint64_t CreateMesh(const MeshDescriptor &desc) override;
    uint64_t CreateTexture(const TextureDescriptor &desc) override;
    uint64_t CreateShader(const ShaderDescriptor &desc) override;

    void DestroyMesh(uint64_t handle) override;
    void DestroyTexture(uint64_t handle) override;
    void DestroyShader(uint64_t handle) override;

    void Cleanup() override {};

private:
    void* window = nullptr;

    struct GLMesh {
        GLuint vbo = 0;
        GLuint ibo = 0;
        VertexLayoutDesc layout;
        GLsizei vertexStride = 0;
    };

    struct GLTexture {
        GLuint id = 0;
    };

    struct GLShader {
        GLuint id = 0;

        // Reflection caches
        std::unordered_map<std::string, GLint> uniformLocations; // plain u_*
        std::unordered_map<std::string, GLuint> uniformBlocks;   // block name -> index
        std::unordered_map<std::string, GLint> samplerUnits;     // sampler name -> unit
        std::unordered_map<std::string, GLint> attribLocations;  // attribute name -> location
    };

    std::vector<RenderCommand> commandQueue;

    std::unordered_map<uint64_t, GLMesh> meshRegistry;
    std::unordered_map<uint64_t, GLTexture> textureRegistry;
    std::unordered_map<uint64_t, GLShader> shaderRegistry;

    // VAO cache (mesh, shader) -> VAO
    std::unordered_map<uint64_t, GLuint> vaoCache;

    // Frame / Camera UBO
    FrameData currentFrameData;
    bool cameraDirty = true;
    GLuint cameraUBO = 0;

    uint64_t nextHandle = 1;

    // ------------ Utility ------------
    uint64_t GenerateHandle();
    void ExecuteCommand(const RenderCommand& cmd);

    // VAO creation based on shader reflection and mesh CPU layout
    GLuint GetOrCreateVAO(uint64_t meshHandle, uint64_t shaderHandle);

    // Shader reflection
    void ReflectShader(GLuint program, GLShader& out);
    static uint64_t MakeKey(uint64_t a, uint64_t b);

    // Binding helpers
    void ApplyUniformAssignments(const GLShader& sh, const std::vector<UniformAssignment>& uniforms);

    // Name -> semantic aliasing for attributes
    static VertexSemantic GuessSemanticFromName(const std::string& n);

    // Data type size and GL enums
    static GLsizei DataTypeByteSize(VertexDataType t);
    static GLenum DataTypeToGL(VertexDataType t);
};



#endif //OPENGLRENDERER_HPP
