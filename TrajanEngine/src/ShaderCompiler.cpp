/*
* File: TrajanEngine
* Project: Trajan
* Author: Collin Longoria
* Created on: 7/15/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#include "ShaderCompiler.hpp"

#include <shaderc/shaderc.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Log.hpp"

namespace {
    shaderc_shader_kind GetShadercKind(ShaderStage stage) {
        switch (stage) {
            case ShaderStage::Vertex: return shaderc_glsl_vertex_shader;
            case ShaderStage::Fragment: return shaderc_glsl_fragment_shader;
            case ShaderStage::Compute: return shaderc_glsl_compute_shader;
            case ShaderStage::Geometry: return shaderc_glsl_geometry_shader;
            case ShaderStage::TessControl: return shaderc_glsl_tess_control_shader;
            case ShaderStage::TessEval: return shaderc_glsl_tess_evaluation_shader;
            default: return shaderc_glsl_infer_from_source;
        }
    }

    std::string ReadFile(const std::string& path) {
        std::ifstream file(path);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
}

std::optional<CompiledShader> CompileShaderToSpirv(const ShaderCompileOptions& options) {
    shaderc::Compiler compiler;
    shaderc::CompileOptions compileOptions;

    for(const auto& def : options.defines) {
        compileOptions.AddMacroDefinition(def);
    }

    compileOptions.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_4);
    compileOptions.SetOptimizationLevel(shaderc_optimization_level_performance);

    const std::string source = ReadFile(options.sourcePath);
    shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(
        source, GetShadercKind(options.stage), options.sourcePath.c_str(),
        options.entryPoint.c_str(), compileOptions
    );

    if( result.GetCompilationStatus() != shaderc_compilation_status_success ) {
        Log::Error("SHADER COMPILATION ERROR: " + result.GetErrorMessage());
        return std::nullopt;
    }

    return CompiledShader {
        .spirv = { result.cbegin(), result.cend() },
        .warnings = result.GetErrorMessage()
    };
}

bool WriteSpirvToFile(const std::string& path, const std::vector<uint32_t>& spirv) {
    std::ofstream file(path, std::ios::binary | std::ios::out);
    if(!file.is_open()) {
        Log::Error("WriteSpirvToFile(): Failed to open file for writing");
        return false;
    }

    file.write(reinterpret_cast<const char*>(spirv.data()), spirv.size() * sizeof(uint32_t));
    if(!file.good()) {
        Log::Error("WriteSpirvToFile(): Failed to write to file");
        return false;
    }

    file.close();
    return true;
}