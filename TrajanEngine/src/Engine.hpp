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

#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <TrajanEngine.hpp>
#include "IRenderer.hpp"

class Window;
class IRenderer;
class Orchestrator;

namespace Trajan {
    class TRAJANENGINE_API Engine {
    public:
        Engine() = default;
        ~Engine() = default;

        void Initialize(int width, int height, const std::string& name, RenderAPI api);

        void Update(float dt);
        void Shutdown();

        void RequestShutdown() { bShouldClose = true; };
        [[nodiscard]] bool ShouldShutdown() const;

        [[nodiscard]] IRenderer* GetRenderer() const { return mRenderer.get(); }

    private:
        bool bShouldClose = false;

        RenderAPI mActiveAPI = RenderAPI::OpenGL; // Default to OpenGL

        std::shared_ptr<Orchestrator> mOrchestrator;
        std::shared_ptr<IRenderer> mRenderer;
        std::shared_ptr<Window> mWindow;
    };
}

#endif //ENGINE_HPP