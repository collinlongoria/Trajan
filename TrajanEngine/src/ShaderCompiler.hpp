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

#ifndef SHADERCOMPILER_HPP
#define SHADERCOMPILER_HPP

#include "TrajanEngine.hpp"

#include <vector>
#include <string>
#include <optional>

enum class ShaderStage {
    Vertex,
    Fragment,
    Compute,
    Geometry,
    TessControl,
    TessEval
};

struct CompiledShader {
  std::vector<uint32_t> spirv;
    std::string warnings;
};

struct ShaderCompileOptions {
    ShaderStage stage;
    std::string sourcePath;
    std::string entryPoint = "main";
    std::string targetEnv = "vulkan1.4";
    std::vector<std::string> defines;
};

TRAJANENGINE_API std::optional<CompiledShader> CompileShaderToSpirv(const ShaderCompileOptions& options);

TRAJANENGINE_API bool WriteSpirvToFile(const std::string& path, const std::vector<uint32_t>& spirv); // <-- will be removed when I make an asset manager

#endif //SHADERCOMPILER_HPP
