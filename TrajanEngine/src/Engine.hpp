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

#include <Window.hpp>

class IRenderer;

class Engine {
public:
    Engine() = default;
    ~Engine() = default;

    void Initialize();
    void Update(float dt);
    void Shutdown();

    void CreateWindow(const std::string& windowName, uint32_t width, uint32_t height);

    void CallShutdown();
    bool ShouldClose();
private:
    bool bInitialized = false;
    bool bShouldClose = false;

    std::shared_ptr<IRenderer> mRenderer;
    std::shared_ptr<Window> mWindow;
};

std::shared_ptr<Engine> GetEngine();

inline void Trajan::Initialize() {
    GetEngine()->Initialize();
}

inline void Trajan::CreateWindow(uint32_t width, uint32_t height, const std::string &name) {
    GetEngine()->CreateWindow(name, width, height);
}

inline void Trajan::Shutdown() {
    GetEngine()->Shutdown();
}

inline bool Trajan::ShouldClose() {
    return GetEngine()->ShouldClose();
}

inline void Trajan::Update(float dt) {
    GetEngine()->Update(dt);
}







#endif //ENGINE_HPP
