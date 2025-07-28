/*
* File: EntityManager.hpp
* Project: Trajan
* Author: ${AUTHOR}
* Created on: 7/26/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#ifndef ENTITYMANAGER_HPP
#define ENTITYMANAGER_HPP
#include <array>
#include <optional>
#include <queue>

#include "component.hpp"
#include "entity.hpp"
#include "log.hpp"

class EntityManager {
public:
    EntityManager() {
        // Initialize the queue with all possible entity IDs
        for(Entity e = 0; e < MAX_ENTITIES; ++e) {
            availableEntities.push(e);
        }
    }

    ~EntityManager() = default;

    std::optional<Entity> CreateEntity() {
        if(livingEntities >= MAX_ENTITIES) {
            Log::Error("Entity count exceeded!");
            return std::nullopt;
        }

        // Get ID from the front of the queue
        Entity id = availableEntities.front();
        availableEntities.pop();
        ++livingEntities;

        return id;
    }

    void DestroyEntity(Entity entity) {
        if( entity >= MAX_ENTITIES ) {
            Log::Error("Tried to destroy out-of-range entity ID " + std::to_string(entity));
            return;
        }

        // Invalidate signature
        signatures[entity].reset();

        // Put the destroyed ID at the back of the queue
        availableEntities.push(entity);
        --livingEntities;
    }

    void SetSignature(Entity entity, Signature signature) {
        if( entity >= MAX_ENTITIES ) {
            Log::Error("Entity ID " + std::to_string(entity) + " out of range!");
            return;
        }

        // Put this entity's signature into the array
        signatures[entity] = signature;
    }

    std::optional<Signature> GetSignature(Entity entity) {
        if( entity >= MAX_ENTITIES ) {
            Log::Error("Entity ID " + std::to_string(entity) + " out of range!");
            return std::nullopt;
        }

        // Get the entity's signature from the array
        return signatures[entity];
    }

private:
    // Queue of unused entity ID
    std::queue<Entity> availableEntities;

    // Array of signatures <- Index corresponds to entity ID
    std::array<Signature, MAX_ENTITIES> signatures{};

    // Total living entity count
    uint32_t livingEntities{};
};

#endif //ENTITYMANAGER_HPP
