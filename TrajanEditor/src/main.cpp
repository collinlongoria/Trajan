#include <chrono>
#include <iostream>
#include <ostream>

#include <trajan_engine.hpp>
#include <imgui.h>
#include <uuid.hpp>
#include <sprite.hpp>
#include <transform_2d.hpp>
#include <orchestrator.hpp>
#include <engine.hpp>
#include <asset_system.hpp>
#include <i_renderer.hpp>

#include "mesh_manager.hpp"
#include "shader.hpp"
#include "shader_manager.hpp"

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

    auto renderer = engine->GetRenderer();
    auto ecs = engine->GetOrchestrator();
    auto assets = engine->GetAssetSystem();

    auto& meshMgr = assets->Get<MeshManager>();
    auto& shaderMgr = assets->Get<ShaderManager>();

    ImGui::SetCurrentContext(renderer->GetImGuiContext());

    /*
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
    */
    // Create Entity
    Entity joe = ecs->CreateEntity();

    ecs->AddComponent(joe, Transform2D({
        .position = Vector2(400.0, 300.0),
        .rotation = 0.0f,
        .scale = Vector2(40.0f, 40.0f),
    }));

    // Create a simple quad using sprite
    Sprite s;
    s.mesh = meshMgr.loadQuad();

    ShaderDescriptor shaderDesc = {
        .vertexSource = vertexShaderSource,
        .fragmentSource = fragmentShaderSource
    };
    s.shader = shaderMgr.getOrCreate("flat", shaderDesc);

    ecs->AddComponent(joe, s);

    // UUID test
    UUID id = UUID::generate();
    Log::Message(id.toString());

    float angle = 0.0f;

    float dt = 0.0f;
    while(!engine->ShouldShutdown()) {
        auto start = std::chrono::high_resolution_clock::now();

        FrameData fd;
        fd.view = Matrix4(1.0f);
        fd.proj = glm::ortho(0.0f, 800.0f, 600.0f, 0.0f, -1.0f, 1.0f);
        fd.cameraPos = Vector3(0.0f, 0.0f, 0.0f);
        renderer->SetFrameData(fd); // Frame data can be set at any time

        // Rotating Entity
        angle += dt;
        auto& t = ecs->GetComponent<Transform2D>(joe);
        t.rotation = angle;

        // MUST begin frame before sending any render commands or drawing GUI
        engine->BeginFrame();

        engine->Update(dt);

        // Test: imgui
        ImGui::Begin("Test");
        ImGui::Text("Hello World!");
        ImGui::End();

        // Must end frame after any GUI or application draws
        engine->EndFrame();

        auto stop = std::chrono::high_resolution_clock::now();

        dt = std::chrono::duration_cast<std::chrono::duration<float>>(stop - start).count();
    }

    engine->Shutdown();

    return 0;
}