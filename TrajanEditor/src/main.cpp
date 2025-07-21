#include <chrono>
#include <iostream>
#include <ostream>

#include <TrajanEngine.hpp>
#include <Log.hpp>
#include "ShaderCompiler.hpp"


int main(void) {

    // UNIT TEST: Shader Compilation
    auto vertResult = CompileGLSLtoSPIRV("assets/shaders/triangle.vert.glsl",ShaderStage::Vertex);
    auto fragResult = CompileGLSLtoSPIRV("assets/shaders/triangle.frag.glsl",ShaderStage::Fragment);

    if(vertResult && fragResult) {
        WriteSpirvToFile("assets/shaders/triangle.vert.spv", vertResult.value());
        WriteSpirvToFile("assets/shaders/triangle.frag.spv", fragResult.value());
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