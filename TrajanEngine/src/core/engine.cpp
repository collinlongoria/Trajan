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

#include "engine.hpp"
#include <log.hpp>
#include <window.hpp>
#include <opengl_renderer.hpp>

#include "orchestrator.hpp"

namespace Trajan {

    // Factory function, exported by the library
    std::unique_ptr<Engine> CreateEngine() {
        return std::make_unique<Engine>();
    }

    // Engine member functions

    void Engine::Update(float dt) {
        if(mWindow) mWindow->PollEvents();
        //if(mRenderer) mRenderer->Render(dt);
    }

    void Engine::Shutdown() {
        Log::Message("Shutting down engine...");

        // Terminate renderer
        if(mRenderer) mRenderer->Cleanup();
        mWindow.reset(); // <-- may work without this?

        Log::Message("Engine shutdown complete!");
    }

    void Engine::Initialize(int width, int height, const std::string& name, RenderAPI api) {
        mActiveAPI = api;

        mWindow = std::make_shared<Window>(width, height, name, api);

        if(!mWindow) {
            Log::Error("Failed to create window");
            return;
        }

        RendererInitInfo info = {
            .nativeWindowHandle = mWindow->NativeWindow(),
            .width              = mWindow->Width(),
            .height             = mWindow->Height(),
            .preferredAPI       = api
        };

        // Choose renderer backend
        // TODO: This fails in terms of modifiability. Consider refactor.
        switch( api ) {
            case RenderAPI::OpenGL:
                Log::Message( "Initializing OpenGL Renderer..." );
                mRenderer = std::make_shared<OpenGLRenderer>();
                break;
            default:
                Log::Error("Unsupported render API requested");
                return;
        }

        Log::Message( "Initialized renderer..." );
        mRenderer->Initialize(info);

        Log::Message( "Initializing ECS Orchestrator..." );
        mOrchestrator = std::make_shared<Orchestrator>();
        mOrchestrator->Initialize();

        Log::Message( "Engine initialized!" );
    }

    bool Engine::ShouldShutdown() const {
        return bShouldClose || (mWindow ? mWindow->ShouldClose() : true);
    }
}
