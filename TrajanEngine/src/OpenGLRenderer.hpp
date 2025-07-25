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
        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ibo = 0;
    };

    struct GLTexture {
        GLuint id = 0;
    };

    struct GLShader {
        GLuint id = 0;
    };

    std::vector<RenderCommand> commandQueue;

    std::unordered_map<uint64_t, GLMesh> meshRegistry;
    std::unordered_map<uint64_t, GLTexture> textureRegistry;
    std::unordered_map<uint64_t, GLShader> shaderRegistry;

    uint64_t nextHandle = 1;

    // Utility
    uint64_t GenerateHandle();
    void ExecuteCommand(const RenderCommand& cmd);
};



#endif //OPENGLRENDERER_HPP
