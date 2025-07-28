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

#ifndef SHADER_COMPILER_HPP
#define SHADER_COMPILER_HPP

#include "trajan_engine.hpp"

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

TRAJANENGINE_API std::optional<std::vector<uint32_t>> CompileGLSLtoSPIRV(const std::string& source, ShaderStage stage);

TRAJANENGINE_API bool WriteSpirvToFile(const std::string& path, const std::vector<uint32_t>& spirv); // <-- will be removed when I make an asset manager

#endif //SHADERCOMPILER_HPP
