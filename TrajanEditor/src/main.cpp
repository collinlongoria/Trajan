#include <chrono>
#include <iostream>
#include <ostream>

#include <TrajanEngine.hpp>
#include <Log.hpp>

#include "OpenGLRenderer.hpp"
#include "ShaderCompiler.hpp"

// UNIT TEST: Hello Triangle Data
static const float vertices[] = {
    0.0f, 0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
};

static const uint32_t indices[] = {
    0, 1, 2
};

const char* vertexShaderSource = R"(
#version 460 core

layout(std140) uniform Camera {
    mat4 u_View;
    mat4 u_Proj;
    vec4 u_CameraPos; // xyz used
};

uniform mat4 u_Model;

in vec3 aPosition;

void main() {
    gl_Position = u_Proj * u_View * u_Model * vec4(aPosition, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 460 core
out vec4 FragColor;

void main() {
    FragColor = vec4(1.0, 0.3, 0.2, 1.0); // red/orange
}
)";

int main(void) {

    // UNIT TEST: Shader Compilation
    /*
    auto vertResult = CompileGLSLtoSPIRV("assets/shaders/triangle.vert.glsl",ShaderStage::Vertex);
    auto fragResult = CompileGLSLtoSPIRV("assets/shaders/triangle.frag.glsl",ShaderStage::Fragment);

    if(vertResult && fragResult) {
        WriteSpirvToFile("assets/shaders/triangle.vert.spv", vertResult.value());
        WriteSpirvToFile("assets/shaders/triangle.frag.spv", fragResult.value());
        Log::Message("Compiled and saved both shaders!");
    }
    */

    // Create engine variable
    auto engine = Trajan::CreateEngine();

    engine->Initialize(800, 600, "Trajan Editor", RenderAPI::OpenGL);

    auto renderer = static_cast<OpenGLRenderer*>(engine->GetRenderer());

    MeshDescriptor meshDesc = {
        .vertexData = vertices,
        .vertexSize = sizeof(vertices),
        .indexData = indices,
        .indexSize = sizeof(indices),
    };

    meshDesc.layout.stride = sizeof(float) * 3;
    meshDesc.layout.attribs.push_back({
        VertexSemantic::Position,           // semantic
        VertexDataType::Float32,            // type
        3,                                  // components
        false,                              // normalized
        0                                   // offset
    });

    uint64_t meshHandle = renderer->CreateMesh(meshDesc);

    ShaderDescriptor shaderDesc = {
        .vertexSource = vertexShaderSource,
        .fragmentSource = fragmentShaderSource
    };
    uint64_t shaderHandle = renderer->CreateShader(shaderDesc);

    Mesh triangleMesh { .vertexCount = 3, .indexCount = 3, .rendererHandle = meshHandle };
    Shader triangleShader { .rendererHandle = shaderHandle };

    float angle = 0.0f;

    float dt = 0.0f;
    while(!engine->ShouldShutdown()) {
        auto start = std::chrono::high_resolution_clock::now();

        FrameData fd;
        fd.view = Matrix4(1.0f);
        fd.proj = Matrix4(1.0f);
        fd.cameraPos = Vector3(0.0f, 0.0f, 0.0f);
        renderer->SetFrameData(fd);

        renderer->BeginFrame();

        angle += dt;
        Matrix4 model = Matrix4(1.0f);
        model = glm::rotate(model, angle, Vector3(0.0f, 0.0f, 1.0f));

        RenderCommand cmd;
        cmd.type = RenderCommand::Type::Mesh;
        cmd.mesh = &triangleMesh;
        cmd.shader = &triangleShader;
        cmd.transform = model;

        renderer->SubmitRenderCommand(cmd);
        renderer->EndFrame();

        engine->Update(dt);

        auto stop = std::chrono::high_resolution_clock::now();

        dt = std::chrono::duration_cast<std::chrono::duration<float>>(stop - start).count();
    }

    engine->Shutdown();

    return 0;
}