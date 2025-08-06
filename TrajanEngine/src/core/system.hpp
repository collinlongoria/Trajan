/*
* File: System.hpp
* Project: Trajan
* Author: ${AUTHOR}
* Created on: 7/26/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#ifndef SYSTEM_HPP
#define SYSTEM_HPP
#include <set>

#include "entity.hpp"

class Window;
class Orchestrator;
class IRenderer;

// System context contains references to potentially useful references from the engine
struct SystemContext {
    Orchestrator& orchestrator;
    IRenderer& renderer;
    Window& window;
    // TODO: when I set up EventBus, put it here
};

class System {
public:
    virtual ~System() = default;

    virtual void Initialize(const SystemContext& ctx) = 0;
    virtual void Update(float dt) = 0;
    virtual void Shutdown() = 0;

    std::set<Entity> entities;
};

#endif //SYSTEM_HPP
