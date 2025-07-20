#include <chrono>
#include <iostream>
#include <ostream>

#include <TrajanEngine.hpp>
#include <Log.hpp>
#include "ShaderCompiler.hpp"


int main(void) {

    // UNIT TEST: Shader Compilation
    ShaderCompileOptions vertOptions{
        .stage = ShaderStage::Vertex,
        .sourcePath = "assets/shaders/test_shader.vert.glsl"
    };

    ShaderCompileOptions fragOptions{
        .stage = ShaderStage::Fragment,
        .sourcePath = "assets/shaders/test_shader.frag.glsl"
    };

    auto vertResult = CompileShaderToSpirv(vertOptions);
    auto fragResult = CompileShaderToSpirv(fragOptions);

    if(vertResult && fragResult) {
        WriteSpirvToFile("assets/shaders/test_shader.vert.spv", vertResult->spirv);
        WriteSpirvToFile("assets/shaders/test_shader.frag.spv", fragResult->spirv);
        Log::Message("Compiled and saved both shaders!");
    }

    // Create engine variable
    auto engine = Trajan::CreateEngine();

    engine->Initialize();
    engine->CreateWindow("Trajan Editor", 800, 600);

    float dt = 0.0f;
    while(!engine->ShouldShutdown()) {
        auto start = std::chrono::high_resolution_clock::now();

        engine->Update(dt);

        auto stop = std::chrono::high_resolution_clock::now();

        dt = std::chrono::duration_cast<std::chrono::duration<float>>(stop - start).count();
    }

    engine->Shutdown();

    return 0;
}