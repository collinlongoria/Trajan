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
#include "render_system.hpp"
#include "sprite.hpp"
#include "transform_2d.hpp"

#include "mesh_manager.hpp"
#include "shader_manager.hpp"

namespace Trajan {

    // Factory function, exported by the library
    std::unique_ptr<Engine> CreateEngine() {
        return std::make_unique<Engine>();
    }

    // Engine member functions

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

        Log::Message("Initializing Asset System");
        mAssetSystem = std::make_shared<AssetSystem>();

        Log::Message("Initializing Mesh Manager..." );
        auto& meshMgr = mAssetSystem->EmplaceManager<MeshManager>(*mRenderer);

        Log::Message("Initializing Shader Manager...");
        auto& shaderMgr = mAssetSystem->EmplaceManager<ShaderManager>(*mRenderer);

        Log::Message( "Initializing ECS Orchestrator..." );
        mOrchestrator = std::make_shared<Orchestrator>();
        mOrchestrator->Initialize();

        Log::Message("Registering components and systems...");
        mOrchestrator->RegisterComponent<Transform2D>();
        mOrchestrator->RegisterComponent<Sprite>();

        mOrchestrator->CreateSystem<RenderSystem, Sprite, Transform2D>();

        // Build init context and init systems
        SystemContext ctx{
            .orchestrator = *mOrchestrator,
            .renderer = *mRenderer,
            .window = *mWindow
        };
        mOrchestrator->InitializeSystems(ctx);

        Log::Message( "Engine initialized!" );
    }

    void Engine::BeginFrame() {
        mRenderer->BeginFrame();
    }

    void Engine::Update(float dt) {
        if(mWindow) mWindow->PollEvents();

        // Update all systems
        mOrchestrator->UpdateSystems(dt);
    }

    void Engine::EndFrame() {
        mAssetSystem->CollectGarbage();
        mRenderer->EndFrame();
    }

    void Engine::Shutdown() {
        Log::Message("Shutting down engine...");

        // Cleanup all Systems
        mOrchestrator->ShutdownSystems();

        // Cleanup all assets
        mAssetSystem->UnloadAssets();

        // Terminate renderer
        if(mRenderer) mRenderer->Cleanup();
        mWindow.reset(); // <-- may work without this?

        Log::Message("Engine shutdown complete!");
    }



    bool Engine::ShouldShutdown() const {
        return bShouldClose || (mWindow ? mWindow->ShouldClose() : true);
    }
}
