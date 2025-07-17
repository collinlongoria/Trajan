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
#include <VulkanRenderer.hpp>

std::shared_ptr<Engine> GetEngine() {
    static std::shared_ptr<Engine> s_instance = std::make_shared<Engine>();
    return s_instance;
}

void Engine::Initialize() {
    if(bInitialized) {
        Log::Warn("Engine Initialize already called Once");
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
    Log::Message("Shutting Down Engine");

    // Terminate renderer
    mRenderer->Shutdown();

    // Destroy any windows
    // TODO: This currently will not work, if a reference to the window exists elsewhere
    if(mWindow) mWindow.reset();

    Log::Message("Goodbye!");
}

void Engine::CreateWindow(const std::string &windowName, uint32_t width, uint32_t height) {
    mWindow = std::make_shared<Window>(width, height, windowName);

    if(mRenderer) mRenderer->WindowInitialization(mWindow->NativeWindow());

    if(!mWindow) {
        Log::Error("Failed to create window");
    }
}


void Engine::CallShutdown() {
    bShouldClose = true;
}


bool Engine::ShouldClose() {
    return (mWindow.get()->ShouldClose() || bShouldClose);
}



