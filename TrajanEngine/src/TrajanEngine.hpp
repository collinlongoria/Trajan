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
    class Engine; // Opaque forward declaration

    [[nodiscard]] TRAJANENGINE_API std::unique_ptr<Engine> CreateEngine();
}

// Suppress MSVC warning about DLL interface for STL classes
// (no longer relevant since I am not using MSVC LOL)
#ifdef _MSC_VER
#pragma warning(disable:4251)
#endif

#include "Engine.hpp"
#endif //TRAJANENGINE_HPP
