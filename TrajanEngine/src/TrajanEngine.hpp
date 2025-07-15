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

#ifndef TRAJANENGINE_HPP
#define TRAJANENGINE_HPP

#include "ILogger.hpp"

#ifdef _WIN32
    #ifdef TRAJANENGINE_EXPORTS
        #define TRAJANENGINE_API __declspec(dllexport)
    #else
        #define TRAJANENGINE_API __declspec(dllimport)
    #endif
#elif defined(__GNUC__) && __GNUC__ >= 4
    #define TRAJANENGINE_API __attribute__((visibility("default")))
#else
    #define TRAJANENGINE_API
#endif

namespace Trajan {
    // Logger
    TRAJANENGINE_API std::shared_ptr<ILogger> GetLogger();

    // Engine
    TRAJANENGINE_API void Initialize();
    TRAJANENGINE_API void CreateWindow(uint32_t width, uint32_t height, const std::string& name);
    TRAJANENGINE_API void Update(float dt);
    TRAJANENGINE_API bool ShouldClose();
    TRAJANENGINE_API void Shutdown();
}

// Suppress MSVC warning about DLL interface for STL classes
// (no longer relevant since I am not using MSVC LOL)
#ifdef _MSC_VER
#pragma warning(disable:4251)
#endif

#endif //TRAJANENGINE_HPP
