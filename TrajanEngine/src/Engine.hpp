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

class Window;
class IRenderer;

namespace Trajan {
    class TRAJANENGINE_API Engine {
    public:
        Engine() = default;
        ~Engine() = default;

        void Initialize();
        void Update(float dt);
        void Shutdown();

        void CreateWindow(const std::string& windowName, uint32_t width, uint32_t height);

        void RequestShutdown() { bShouldClose = true; };
        bool ShouldShutdown() const;
    private:
        bool bInitialized = false;
        bool bShouldClose = false;

        std::shared_ptr<IRenderer> mRenderer;
        std::shared_ptr<Window> mWindow;
    };
}

#endif //ENGINE_HPP