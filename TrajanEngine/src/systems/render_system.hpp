/*
* File: render_system.hpp
* Project: Trajan
* Author: ${AUTHOR}
* Created on: 8/1/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#ifndef RENDER_SYSTEM_HPP
#define RENDER_SYSTEM_HPP
#include "system.hpp"

class Orchestrator;
class IRenderer;

class RenderSystem : public System {
public:
    void Initialize(const SystemContext& ctx) override;

    void Update(float dt) override;

    void Shutdown() override {};

private:
    IRenderer* mRenderer = nullptr;
    Orchestrator* mOrchestrator = nullptr;
};

#endif //RENDER_SYSTEM_HPP
