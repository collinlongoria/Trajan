/*
* File: Engine
* Project: Trajan
* Author: colli
* Created on: 7/15/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#include "Engine.hpp"
#include <Log.hpp>
#include <Window.hpp>
#include <VulkanRenderer.hpp>

namespace Trajan {

    // Factory function, exported by the library
    std::unique_ptr<Engine> CreateEngine() {
        return std::make_unique<Engine>();
    }

    // Engine member functions
    void Engine::Initialize() {
        if(bInitialized) {
            Log::Warn("Engine::Initialize() called more than once");
            return;
        }

        mRenderer = std::make_shared<VulkanRenderer>();
        mRenderer->Initialize();

        bShouldClose = false;
        bInitialized = true;
    }

    void Engine::Update(float dt) {
        if(mWindow) mWindow->PollEvents();
    }

    void Engine::Shutdown() {
        Log::Message("Shutting down engine...");

        // Terminate renderer
        if(mRenderer) mRenderer->Shutdown();
        mWindow.reset(); // <-- may work without this?

        Log::Message("Engine shutdown complete!");
    }

    void Engine::CreateWindow(const std::string &windowName, uint32_t width, uint32_t height) {
        mWindow = std::make_shared<Window>(width, height, windowName);

        if(mRenderer) mRenderer->WindowInitialization(mWindow->NativeWindow());

        if(!mWindow) {
            Log::Error("Failed to create window");
        }
    }

    bool Engine::ShouldShutdown() const {
        return bShouldClose || (mWindow ? mWindow->ShouldClose() : true);
    }
}