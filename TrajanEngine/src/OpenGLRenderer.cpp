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

#include "Log.hpp"
#include "Texture.hpp"

// Fixed binding points for frame data
static constexpr GLuint CAMERA_BINDING = 0;

// Helpers
uint64_t OpenGLRenderer::GenerateHandle() {
    return nextHandle++;
}

uint64_t OpenGLRenderer::MakeKey(uint64_t a, uint64_t b) {
    // Simple 64-bit hash combine
    // TODO: Could this be moved elsewhere?
    return (a) ^ (b + 0x9e3779b97f4a7c13 + (a << 6) + (a >> 2));
}

GLsizei OpenGLRenderer::DataTypeByteSize(VertexDataType t) {
    switch (t) {
        case VertexDataType::Float32:   return 4;
        case VertexDataType::HalfFloat: return 2;
        case VertexDataType::UInt8:     return 1;
        case VertexDataType::UInt16:    return 2;
        case VertexDataType::UInt32:    return 4;
        case VertexDataType::Int8:      return 1;
        case VertexDataType::Int16:     return 2;
        case VertexDataType::Int32:     return 4;
        case VertexDataType::UNorm8:    return 1;
        case VertexDataType::UNorm16:   return 2;
        default: return 4;
    }
}

GLenum OpenGLRenderer::DataTypeToGL(VertexDataType t) {
    switch (t) {
        case VertexDataType::Float32:   return GL_FLOAT;
        case VertexDataType::HalfFloat: return GL_HALF_FLOAT;
        case VertexDataType::UInt8:     return GL_UNSIGNED_BYTE;
        case VertexDataType::UInt16:    return GL_UNSIGNED_SHORT;
        case VertexDataType::UInt32:    return GL_UNSIGNED_INT;
        case VertexDataType::Int8:      return GL_BYTE;
        case VertexDataType::Int16:     return GL_SHORT;
        case VertexDataType::Int32:     return GL_INT;
        case VertexDataType::UNorm8:    return GL_UNSIGNED_BYTE;
        case VertexDataType::UNorm16:   return GL_UNSIGNED_SHORT;
        default: return GL_FLOAT;
    }
}

// TODO: Note, this helper is actually kind of awful. But it will work for now I suppose
VertexSemantic OpenGLRenderer::GuessSemanticFromName(const std::string &n) {
    // lower-case for simplicity
    auto lower = n; std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    if( lower == "apos" || lower == "position" || lower == "in_pos" || lower == "inposition"
        || lower == "aposition" || lower == "a_position" )
        return VertexSemantic::Position;

    if (lower == "anormal" || lower == "normal" || lower == "in_normal" || lower == "a_normal")
        return VertexSemantic::Normal;

    if (lower == "atangent" || lower == "tangent" || lower == "in_tangent" || lower == "a_tangent")
        return VertexSemantic::Tangent;

    if (lower == "abitangent" || lower == "bitangent" || lower == "in_bitangent" || lower == "a_bitangent")
        return VertexSemantic::Bitangent;

    if (lower == "acolor" || lower == "color" || lower == "in_color" || lower == "a_color" || lower == "color0" || lower == "acolor0")
        return VertexSemantic::Color0;

    if (lower == "atexcoord" || lower == "atexcoord0" || lower == "texcoord" || lower == "texcoord0" ||
        lower == "uv" || lower == "uv0" || lower == "in_uv" || lower == "a_uv")
        return VertexSemantic::TexCoord0;

    if (lower == "texcoord1" || lower == "uv1" || lower == "a_texcoord1" || lower == "in_uv1")
        return VertexSemantic::TexCoord1;

    return VertexSemantic::Custom;
}

void OpenGLRenderer::Initialize(const RendererInitInfo &initInfo) {
    window = initInfo.nativeWindowHandle;

    glfwMakeContextCurrent(static_cast<GLFWwindow *>(window));
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glViewport(0, 0, initInfo.width, initInfo.height);
    glEnable(GL_DEPTH_TEST);

    // Create frame data UBO
    glGenBuffers(1, &cameraUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
    const GLsizeiptr camSize = sizeof(float) * (16 + 16 + 4);
    glBufferData(GL_UNIFORM_BUFFER, camSize, nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, CAMERA_BINDING, cameraUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLRenderer::Resize(uint32_t width, uint32_t height) {
    glViewport(0, 0, width, height);
}

void OpenGLRenderer::SetFrameData(const FrameData &fd) {
    currentFrameData = fd;
    cameraDirty = true;
}

void OpenGLRenderer::BeginFrame() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    commandQueue.clear();

    // Update camera
    if( cameraDirty ) {
        // std-140 friendly, consider 340
        glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);

        // Matrix4 is column-major and contiguous
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float)*16, &currentFrameData.view[0][0]);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(float)*16, sizeof(float)*16, &currentFrameData.proj[0][0]);

        float cam[4] = { currentFrameData.cameraPos.x, currentFrameData.cameraPos.y, currentFrameData.cameraPos.z, 0.0f };
        glBufferSubData( GL_UNIFORM_BUFFER, sizeof(float)*32, sizeof(float)*4, cam );

        glBindBuffer( GL_UNIFORM_BUFFER, 0 );
        cameraDirty = false;
    }
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
        case RenderCommand::Type::Mesh: {
            if(! (cmd.mesh && cmd.mesh->rendererHandle && cmd.shader && cmd.shader->rendererHandle) )
                return;

            const auto& sh = shaderRegistry.at( cmd.shader->rendererHandle );
            glUseProgram(sh.id);

            // TODO: this is legacy
            // Bind texture to the first sampler used by the shader
            if(cmd.texture && cmd.texture->rendererHandle) {
                glActiveTexture( GL_TEXTURE0 );
                glBindTexture( GL_TEXTURE_2D, textureRegistry[cmd.texture->rendererHandle].id );
            }

            // Set per-draw model matrix
            auto itModel = sh.uniformLocations.find( "u_Model" );
            if( itModel != sh.uniformLocations.end() ) {
                glUniformMatrix4fv( itModel->second, 1, GL_FALSE, &cmd.transform[0][0] );
            }

            // Apply any user-provided named uniforms for this specific draw call
            ApplyUniformAssignments( sh, cmd.uniforms );

            // Bind the VAO for (mesh, shader), building if needed
            GLuint vao = GetOrCreateVAO( cmd.mesh->rendererHandle, cmd.shader->rendererHandle );
            glBindVertexArray( vao );

            // Draw
            glDrawElements( GL_TRIANGLES, static_cast<GLsizei>(cmd.mesh->indexCount), GL_UNSIGNED_INT, nullptr );

            glBindVertexArray( 0 );
            glUseProgram( 0 );
            break;
        }

        case RenderCommand::Type::CustomCallback: {
            if( cmd.callback ) cmd.callback();
            break;
        }

        default:
            break;
    }
}

uint64_t OpenGLRenderer::CreateMesh(const MeshDescriptor &desc) {
    GLMesh mesh;

    glGenBuffers(1, &mesh.vbo);
    glGenBuffers(1, &mesh.ibo);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(desc.vertexSize), desc.vertexData, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(desc.indexSize), desc.indexData, GL_STATIC_DRAW);

    // Store CPU layout desc and compute stride (if needed)
    mesh.layout = desc.layout;
    if(mesh.layout.stride == 0) {
        uint32_t maxEnd = 0;
        for(const auto& attrib : mesh.layout.attribs) {
            uint32_t end = attrib.offset + attrib.componentCount * static_cast<uint32_t>(DataTypeByteSize(attrib.type));
            maxEnd = std::max(maxEnd, end);
        }
        mesh.vertexStride = static_cast<GLsizei>(maxEnd);
    }
    else {
        mesh.vertexStride = static_cast<GLsizei>(mesh.layout.stride);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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

// shader error check helpers
static bool CheckShader(GLuint s, const char* stageName) {
    GLint ok = GL_FALSE; glGetShaderiv(s,GL_COMPILE_STATUS,&ok);
    if(!ok) {
        GLint len = 0; glGetShaderiv(s,GL_INFO_LOG_LENGTH,&len);
        std::string log(len, '\0'); glGetShaderInfoLog(s, len, nullptr, log.data());
        Log::Error("[GLSL " + std::string(stageName) + "] " + log);
        return false;
    }
    return true;
}

static bool CheckProgram(GLuint p) {
    GLint ok = GL_FALSE; glGetProgramiv(p,GL_LINK_STATUS,&ok);
    if(!ok) {
        GLint len = 0; glGetProgramiv(p,GL_INFO_LOG_LENGTH,&len);
        std::string log(len, '\0'); glGetProgramInfoLog(p, len, nullptr, log.data());
        Log::Error("[GLSL Link] " + log);
        return false;
    }
    return true;
}

uint64_t OpenGLRenderer::CreateShader(const ShaderDescriptor &desc) {
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    const char* vs = desc.vertexSource.c_str();
    glShaderSource(vertex, 1, &vs, nullptr);
    glCompileShader(vertex);
    if(!CheckShader(vertex, "Vertex")) {
        glDeleteShader(vertex);
        return 0;
    }

    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fs = desc.fragmentSource.c_str();
    glShaderSource(fragment, 1, &fs, nullptr);
    glCompileShader(fragment);
    if(!CheckShader(fragment, "Fragment")) {
        glDeleteShader(fragment);
        glDeleteShader(vertex);
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    if(!CheckProgram(program)) {
        glDeleteProgram(program);
        return 0;
    }

    GLShader shader = { .id = program };

    ReflectShader( program, shader );

    uint64_t handle = GenerateHandle();
    shaderRegistry[handle] = std::move(shader);
    return handle;
}

void OpenGLRenderer::DestroyMesh(uint64_t handle) {
    if( meshRegistry.contains( handle ) ) {
        const auto& mesh = meshRegistry[handle];
        glDeleteBuffers(1, &mesh.vbo);
        glDeleteBuffers(1, &mesh.ibo);

        // Remove any VAOs keyed to this mesh
        std::vector<uint64_t> toErase;
        toErase.reserve( vaoCache.size() );
        for(auto& [key, vao] : vaoCache) {
            // Key is (mesh, shader) combined. Cannot be decoded easily.
            // Can rebuild a new map by checking mesh existence later.
            // TODO: could store a secondary index from mesh-> keys and delete here.
            (void)vao;
        }

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

void OpenGLRenderer::ReflectShader(GLuint program, GLShader &out) {
    // Plain uniforms & sampler uniforms
    GLint numUniforms = 0;
    glGetProgramiv( program, GL_ACTIVE_UNIFORMS, &numUniforms );

    GLint maxNameLen = 0;
    glGetProgramiv( program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLen );
    std::string name(maxNameLen, '\0');

    GLint samplerUnit = 0;
    glUseProgram( program );

    for(GLint i = 0; i < numUniforms; ++i) {
        GLsizei length = 0;
        GLint size = 0;
        GLenum type = 0;
        glGetActiveUniform( program, static_cast<GLuint>(i), maxNameLen, &length, &size, &type, name.data() );
        name.resize(length);

        GLint loc = glGetUniformLocation( program, name.c_str() );
        if(loc < 0) continue; // Possibly belongs to a block or optimized out ????

        // Strip array suffix for consistent mapping
        std::string baseName = name;
        if( auto pos = baseName.find("[0]"); pos != std::string::npos ) {
            baseName = baseName.substr(0, pos);
        }

        out.uniformLocations[baseName] = loc;

        // Samplers
        if( type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE || type == GL_SAMPLER_2D_ARRAY ||
            type == GL_INT_SAMPLER_2D || type == GL_UNSIGNED_INT_SAMPLER_2D) {
            out.samplerUnits[baseName] = samplerUnit;
            glUniform1i( loc, samplerUnit );
            samplerUnit++;
        }
    }

    // Uniform blocks
    GLint numBlocks = 0;
    glGetProgramiv( program, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks );

    GLint maxBlockNameLen = 0;
    glGetProgramiv( program, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &maxBlockNameLen );
    std::string blockName( maxBlockNameLen, '\0' );

    for( GLint bi = 0; bi < numBlocks; ++bi ) {
        GLsizei length = 0;
        glGetActiveUniformBlockName( program, static_cast<GLuint>(bi), maxBlockNameLen, &length, blockName.data() );
        blockName.resize(length);
        out.uniformBlocks[blockName] = static_cast<GLuint>(bi);

        // Attach known blocks to known bindings
        // TODO: I will check for name variation for now, but users will want a better way for this
        std::string lower = blockName; std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if( lower == "camera" || lower == "ucamera" ) {
            glUniformBlockBinding( program, static_cast<GLuint>(bi), CAMERA_BINDING );
        }
    }

    // Vertex attributes ( names -> locations )
    GLint numAttribs = 0;
    glGetProgramiv( program, GL_ACTIVE_ATTRIBUTES, &numAttribs );

    GLint maxAttribNameLen = 0;
    glGetProgramiv( program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribNameLen );
    std::string attribName( maxAttribNameLen, '\0' );

    for( GLint ai = 0; ai < numAttribs; ++ai ) {
        GLsizei length = 0;
        GLint size = 0;
        GLenum type = 0;
        glGetActiveAttrib( program, static_cast<GLuint>(ai), maxAttribNameLen, &length, &size, &type, attribName.data() );
        attribName.resize(length);
        GLint loc = glGetAttribLocation( program, attribName.c_str() );
        if( loc >= 0 ) {
            out.attribLocations[attribName] = loc;
        }
    }

    glUseProgram( 0 );
}

GLuint OpenGLRenderer::GetOrCreateVAO(uint64_t meshHandle, uint64_t shaderHandle) {
    const uint64_t key = MakeKey( meshHandle, shaderHandle );
    auto it = vaoCache.find( key );
    if( it != vaoCache.end() ) return it->second;

    const auto& m = meshRegistry.at( meshHandle );
    const auto& sh = shaderRegistry.at( shaderHandle );

    GLuint vao = 0;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    glBindBuffer( GL_ARRAY_BUFFER, m.vbo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m.ibo );

    // For each active attrib in the shader, find a matching semantic from CPU layout and wire it
    for(const auto& [attrName, loc] : sh.attribLocations) {
        VertexSemantic sem = GuessSemanticFromName( attrName );

        // Find first CPU attribute with that semantic
        const VertexAttrib* found = nullptr;
        for(const auto& a : m.layout.attribs) {;
            if( a.semantic == sem ) {
                found = &a;
                break;
            }
        }
        if( !found ) {
            // Leave disabled, OpenGL should default to 0
            Log::Warn("Couldn't find attribute '" + attrName + "'");
            continue;
        }

        const GLboolean normalized = found->normalized ? GL_TRUE : GL_FALSE;
        const GLenum type = DataTypeToGL( found->type );
        const GLsizei comps = static_cast<GLsizei>( found->componentCount );
        const GLsizei stride = m.vertexStride;
        const void* offset = reinterpret_cast<const void*>(static_cast<uintptr_t>(found->offset)); // holy balls lol

        // Determine if normalization due to floating point is needed
        if( type == GL_INT || type == GL_UNSIGNED_INT || type == GL_SHORT || type == GL_UNSIGNED_SHORT ||
            type == GL_BYTE || type == GL_UNSIGNED_BYTE ) {
            glEnableVertexAttribArray( static_cast<GLuint>(loc) );
            glVertexAttribIPointer( static_cast<GLuint>(loc), comps, type, stride, offset );
        }
        else {
            glEnableVertexAttribArray( static_cast<GLuint>(loc) );
            glVertexAttribPointer( static_cast<GLuint>(loc), comps, type, normalized, stride, offset );
        }
    }

    glBindVertexArray( 0 );
    vaoCache[key] = vao;
    return vao;
}

void OpenGLRenderer::ApplyUniformAssignments(const GLShader &sh, const std::vector<UniformAssignment> &uniforms) {
    for(const auto& u : uniforms) {
        auto it = sh.uniformLocations.find( u.name );
        if( it == sh.uniformLocations.end() ) continue;
        GLint loc = it->second;

        if( std::holds_alternative<int>(u.value) ) {
            glUniform1i( loc, std::get<int>(u.value) );
        }
        else if( std::holds_alternative<float>(u.value) ) {
            glUniform1f( loc, std::get<float>(u.value) );
        }
        else if( std::holds_alternative<Matrix4>(u.value) ) {
            const Matrix4& m = std::get<Matrix4>(u.value);
            glUniformMatrix4fv( loc, 1, GL_FALSE, &m[0][0] );
        }
    }
}
