/*
* File: OpenGLRenderer.cpp
* Project: Trajan
* Author: ${AUTHOR}
* Created on: 7/24/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#include "OpenGLRenderer.hpp"
#include <GLFW/glfw3.h>

void OpenGLRenderer::Initialize(const RendererInitInfo &initInfo) {
    window = initInfo.nativeWindowHandle;

    glfwMakeContextCurrent(static_cast<GLFWwindow *>(window));
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glViewport(0, 0, initInfo.width, initInfo.height);
    glEnable(GL_DEPTH_TEST);
}

void OpenGLRenderer::Resize(uint32_t width, uint32_t height) {
    glViewport(0, 0, width, height);
}

void OpenGLRenderer::BeginFrame() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    commandQueue.clear();
}

void OpenGLRenderer::SubmitRenderCommand(const RenderCommand &cmd) {
    commandQueue.push_back(cmd);
}

void OpenGLRenderer::EndFrame() {
    for(const auto& cmd : commandQueue) {
        ExecuteCommand(cmd);
    }

    glfwSwapBuffers(static_cast<GLFWwindow *>(window));
}

void OpenGLRenderer::ExecuteCommand(const RenderCommand &cmd) {
    switch( cmd.type ) {
        case RenderCommand::Type::Mesh:
            if( cmd.mesh && cmd.mesh->rendererHandle ) {
                const GLMesh& mesh = meshRegistry[cmd.mesh->rendererHandle];
                const GLShader& shader = shaderRegistry[cmd.shader->rendererHandle];

                glUseProgram( shader.id) ;
                glBindVertexArray( mesh.vao );

                // Send transform to shader
                // TODO: more robust uniform handler
                GLint loc = glGetUniformLocation( shader.id, "u_Model" );
                if( loc >= 0 )
                    glUniformMatrix4fv( loc, 1, GL_FALSE, &cmd.transform[0][0] );

                glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(cmd.mesh->indexCount), GL_UNSIGNED_INT, nullptr);
            }
            break;

        case RenderCommand::Type::CustomCallback:
            if ( cmd.callback ) cmd.callback();
            break;

        default:
            break;
    }
}

uint64_t OpenGLRenderer::GenerateHandle() {
    return nextHandle++;
}

uint64_t OpenGLRenderer::CreateMesh(const MeshDescriptor &desc) {
    GLMesh mesh;
    glGenVertexArrays(1, &mesh.vao);
    glGenBuffers(1, &mesh.vbo);
    glGenBuffers(1, &mesh.ibo);

    glBindVertexArray(mesh.vao);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(desc.vertexSize), desc.vertexData, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(desc.indexSize), desc.indexData, GL_STATIC_DRAW);

    // TODO: do not assume layout
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    uint64_t handle = GenerateHandle();
    meshRegistry[handle] = mesh;
    return handle;
}

uint64_t OpenGLRenderer::CreateTexture(const TextureDescriptor &desc) {
    GLTexture tex;
    glGenTextures(1, &tex.id);
    glBindTexture(GL_TEXTURE_2D, tex.id);

    glTexImage2D(GL_TEXTURE_2D, 0, desc.sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8,
                 static_cast<GLsizei>(desc.width), static_cast<GLsizei>(desc.height), 0, GL_RGBA, GL_UNSIGNED_BYTE, desc.pixelData);

    if( desc.generateMipmaps ) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // TODO: Allow for changing this (nearest for pixel art, etc)
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, desc.generateMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glBindTexture(GL_TEXTURE_2D, 0);

    uint64_t handle = GenerateHandle();
    textureRegistry[handle] = tex;
    return handle;
}

uint64_t OpenGLRenderer::CreateShader(const ShaderDescriptor &desc) {
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    const char* vs = desc.vertexSource.c_str();
    glShaderSource(vertex, 1, &vs, nullptr);
    glCompileShader(vertex);

    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fs = desc.fragmentSource.c_str();
    glShaderSource(fragment, 1, &fs, nullptr);
    glCompileShader(fragment);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    GLShader shader = { .id = program };
    uint64_t handle = GenerateHandle();
    shaderRegistry[handle] = shader;
    return handle;
}

void OpenGLRenderer::DestroyMesh(uint64_t handle) {
    if( meshRegistry.contains( handle ) ) {
        const auto& mesh = meshRegistry[handle];
        glDeleteBuffers(1, &mesh.vbo);
        glDeleteBuffers(1, &mesh.ibo);
        glDeleteVertexArrays(1, &mesh.vao);
        meshRegistry.erase(handle);
    }
}

void OpenGLRenderer::DestroyTexture(uint64_t handle) {
    if( textureRegistry.contains( handle ) ) {
        glDeleteTextures(1, &textureRegistry[handle].id);
        textureRegistry.erase(handle);
    }
}

void OpenGLRenderer::DestroyShader(uint64_t handle) {
    if( shaderRegistry.contains( handle ) ) {
        glDeleteProgram(shaderRegistry[handle].id);
        shaderRegistry.erase(handle);
    }
}
