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
#include <fstream>
#include <sstream>

#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>
#include <SPIRV/GlslangToSpv.h>

#include "Log.hpp"
#include "glslang/Public/resource_limits_c.h"

namespace {
    std::optional<std::string> ReadTextFile(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if(!file.is_open()) {
            return std::nullopt;
        }

        std::ostringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }

    EShLanguage ToEShLanguage(ShaderStage stage) {
        switch (stage) {
            case ShaderStage::Vertex:      return EShLangVertex;
            case ShaderStage::Fragment:    return EShLangFragment;
            case ShaderStage::Compute:     return EShLangCompute;
            case ShaderStage::Geometry:    return EShLangGeometry;
            case ShaderStage::TessControl: return EShLangTessControl;
            case ShaderStage::TessEval:    return EShLangTessEvaluation;
            default:                       return EShLangCount;
        }
    }

    static TBuiltInResource InitResources() {
        TBuiltInResource Resources;

        Resources.maxLights                                 = 32;
        Resources.maxClipPlanes                             = 6;
        Resources.maxTextureUnits                           = 32;
        Resources.maxTextureCoords                          = 32;
        Resources.maxVertexAttribs                          = 64;
        Resources.maxVertexUniformComponents                = 4096;
        Resources.maxVaryingFloats                          = 64;
        Resources.maxVertexTextureImageUnits                = 32;
        Resources.maxCombinedTextureImageUnits              = 80;
        Resources.maxTextureImageUnits                      = 32;
        Resources.maxFragmentUniformComponents              = 4096;
        Resources.maxDrawBuffers                            = 32;
        Resources.maxVertexUniformVectors                   = 128;
        Resources.maxVaryingVectors                         = 8;
        Resources.maxFragmentUniformVectors                 = 16;
        Resources.maxVertexOutputVectors                    = 16;
        Resources.maxFragmentInputVectors                   = 15;
        Resources.minProgramTexelOffset                     = -8;
        Resources.maxProgramTexelOffset                     = 7;
        Resources.maxClipDistances                          = 8;
        Resources.maxComputeWorkGroupCountX                 = 65535;
        Resources.maxComputeWorkGroupCountY                 = 65535;
        Resources.maxComputeWorkGroupCountZ                 = 65535;
        Resources.maxComputeWorkGroupSizeX                  = 1024;
        Resources.maxComputeWorkGroupSizeY                  = 1024;
        Resources.maxComputeWorkGroupSizeZ                  = 64;
        Resources.maxComputeUniformComponents               = 1024;
        Resources.maxComputeTextureImageUnits               = 16;
        Resources.maxComputeImageUniforms                   = 8;
        Resources.maxComputeAtomicCounters                  = 8;
        Resources.maxComputeAtomicCounterBuffers            = 1;
        Resources.maxVaryingComponents                      = 60;
        Resources.maxVertexOutputComponents                 = 64;
        Resources.maxGeometryInputComponents                = 64;
        Resources.maxGeometryOutputComponents               = 128;
        Resources.maxFragmentInputComponents                = 128;
        Resources.maxImageUnits                             = 8;
        Resources.maxCombinedImageUnitsAndFragmentOutputs   = 8;
        Resources.maxCombinedShaderOutputResources          = 8;
        Resources.maxImageSamples                           = 0;
        Resources.maxVertexImageUniforms                    = 0;
        Resources.maxTessControlImageUniforms               = 0;
        Resources.maxTessEvaluationImageUniforms            = 0;
        Resources.maxGeometryImageUniforms                  = 0;
        Resources.maxFragmentImageUniforms                  = 8;
        Resources.maxCombinedImageUniforms                  = 8;
        Resources.maxGeometryTextureImageUnits              = 16;
        Resources.maxGeometryOutputVertices                 = 256;
        Resources.maxGeometryTotalOutputComponents          = 1024;
        Resources.maxGeometryUniformComponents              = 1024;
        Resources.maxGeometryVaryingComponents              = 64;
        Resources.maxTessControlInputComponents             = 128;
        Resources.maxTessControlOutputComponents            = 128;
        Resources.maxTessControlTextureImageUnits           = 16;
        Resources.maxTessControlUniformComponents           = 1024;
        Resources.maxTessControlTotalOutputComponents       = 4096;
        Resources.maxTessEvaluationInputComponents          = 128;
        Resources.maxTessEvaluationOutputComponents         = 128;
        Resources.maxTessEvaluationTextureImageUnits        = 16;
        Resources.maxTessEvaluationUniformComponents        = 1024;
        Resources.maxTessPatchComponents                    = 120;
        Resources.maxPatchVertices                          = 32;
        Resources.maxTessGenLevel                           = 64;
        Resources.maxViewports                              = 16;
        Resources.maxVertexAtomicCounters                   = 0;
        Resources.maxTessControlAtomicCounters              = 0;
        Resources.maxTessEvaluationAtomicCounters           = 0;
        Resources.maxGeometryAtomicCounters                 = 0;
        Resources.maxFragmentAtomicCounters                 = 8;
        Resources.maxCombinedAtomicCounters                 = 8;
        Resources.maxAtomicCounterBindings                  = 1;
        Resources.maxVertexAtomicCounterBuffers             = 0;
        Resources.maxTessControlAtomicCounterBuffers        = 0;
        Resources.maxTessEvaluationAtomicCounterBuffers     = 0;
        Resources.maxGeometryAtomicCounterBuffers           = 0;
        Resources.maxFragmentAtomicCounterBuffers           = 1;
        Resources.maxCombinedAtomicCounterBuffers           = 1;
        Resources.maxAtomicCounterBufferSize                = 16384;
        Resources.maxTransformFeedbackBuffers               = 4;
        Resources.maxTransformFeedbackInterleavedComponents = 64;
        Resources.maxCullDistances                          = 8;
        Resources.maxCombinedClipAndCullDistances           = 8;
        Resources.maxSamples                                = 4;
        Resources.maxMeshOutputVerticesNV                   = 256;
        Resources.maxMeshOutputPrimitivesNV                 = 512;
        Resources.maxMeshWorkGroupSizeX_NV                  = 32;
        Resources.maxMeshWorkGroupSizeY_NV                  = 1;
        Resources.maxMeshWorkGroupSizeZ_NV                  = 1;
        Resources.maxTaskWorkGroupSizeX_NV                  = 32;
        Resources.maxTaskWorkGroupSizeY_NV                  = 1;
        Resources.maxTaskWorkGroupSizeZ_NV                  = 1;
        Resources.maxMeshViewCountNV                        = 4;

        Resources.limits.nonInductiveForLoops                 = 1;
        Resources.limits.whileLoops                           = 1;
        Resources.limits.doWhileLoops                         = 1;
        Resources.limits.generalUniformIndexing               = 1;
        Resources.limits.generalAttributeMatrixVectorIndexing = 1;
        Resources.limits.generalVaryingIndexing               = 1;
        Resources.limits.generalSamplerIndexing               = 1;
        Resources.limits.generalVariableIndexing              = 1;
        Resources.limits.generalConstantMatrixVectorIndexing  = 1;

        return Resources;
    }

    const TBuiltInResource DefaultResources = InitResources();
}

std::optional<std::vector<uint32_t>> CompileGLSLtoSPIRV(const std::string& source, ShaderStage stage) {
    static bool initialized = false;
    if (!initialized) {
        glslang::InitializeProcess();
        initialized = true;
    }

    EShLanguage shaderType = ToEShLanguage(stage);
    glslang::TShader shader(shaderType);

    auto srcOpt = ReadTextFile(source);
    if(!srcOpt) {
        Log::Error("Cannot open shader file: " + source);
        return std::nullopt;
    }
    std::string src = std::move(*srcOpt);
    const char* strings[] = { src.c_str() };
    shader.setStrings(strings, 1);
    shader.setEnvInput(glslang::EShSourceGlsl, shaderType, glslang::EShClientVulkan, 450);
    shader.setEnvClient(glslang::EShClientVulkan, glslang::EShTargetVulkan_1_4);
    shader.setEnvTarget(glslang::EShTargetSpv, glslang::EShTargetSpv_1_5);

    int defaultVersion = 450;
    EShMessages messages = EShMsgDefault;
    if (!shader.parse(&DefaultResources, defaultVersion, false, messages)) {
        fprintf(stderr, "[GLSLANG] Shader compile error:\n%s\n", shader.getInfoLog());
        return std::nullopt;
    }

    glslang::TProgram program;
    program.addShader(&shader);

    if (!program.link(messages)) {
        fprintf(stderr, "[GLSLANG] Shader link error:\n%s\n", program.getInfoLog());
        return std::nullopt;
    }

    std::vector<uint32_t> spirv;
    glslang::GlslangToSpv(*program.getIntermediate(shaderType), spirv);

    return spirv;
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