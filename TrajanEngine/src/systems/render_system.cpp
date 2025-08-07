/*
* File: render_system.cpp
* Project: Trajan
* Author: ${AUTHOR}
* Created on: 8/1/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#include "render_system.hpp"

#include "engine.hpp"
#include "i_renderer.hpp"
#include "orchestrator.hpp"
#include "sprite.hpp"
#include "transform_2d.hpp"

void RenderSystem::Initialize(const SystemContext& ctx) {
    mOrchestrator = &ctx.orchestrator;
    mRenderer = &ctx.renderer;
}

void RenderSystem::Update(float dt) {
    for(const auto& entity : entities) {
        const auto& transform = mOrchestrator->GetComponent<Transform2D>(entity);
        const auto& sprite = mOrchestrator->GetComponent<Sprite>(entity);

        // Submit renderable to renderer
        RenderCommand cmd;
        cmd.type = RenderCommand::Type::Mesh;
        cmd.mesh = sprite.mesh.operator->();
        cmd.shader = sprite.shader.operator->();
        cmd.transform = transform.Matrix();

        mRenderer->SubmitRenderCommand(cmd);
    }
}
