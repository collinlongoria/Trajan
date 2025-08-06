/*
* File: SystemManager.hpp
* Project: Trajan
* Author: ${AUTHOR}
* Created on: 7/26/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#ifndef SYSTEM_MANAGER_HPP
#define SYSTEM_MANAGER_HPP
#include <memory>
#include <unordered_map>
#include <typeindex>

#include "component.hpp"
#include "log.hpp"
#include "system.hpp"

class SystemManager {
public:
    template<typename T>
    std::shared_ptr<T> RegisterSystem() {
        auto key = std::type_index(typeid(T));

        if( systems.contains( key ) ) {
            Log::Error("Tried to register system type " + std::string(typeid(T).name()) + " more than once!");
            return nullptr;
        }

        // Create and return pointer to the system
        auto system = std::make_shared<T>();
        systems.emplace( key, system );
        order.emplace_back( system );
        return system;
    }

    template<typename T>
    void SetSignature(Signature signature) {
        auto key = std::type_index(typeid(T));

        if( !systems.contains( key ) ) {
            Log::Error("System type " + std::string(typeid(T).name()) + " not registered!");
            return;
        }

        // Set signature for the system
        signatures[key] = signature;
    }

    void InitializeSystems(const SystemContext& ctx) {
        for(auto& sys : order) {
            sys->Initialize(ctx);
        }
    }

    void UpdateSystems(float dt) {
        for(auto& sys : order) {
            sys->Update(dt);
        }
    }

    void ShutdownSystems() {
        for(auto& sys : order) {
            sys->Shutdown();
        }
    }

    void EntityDestroyed(Entity entity) {
        // Erase destroyed entity from all system lists
        for( auto const& pair : systems ) {
            auto const& system = pair.second;

            system->entities.erase( entity );
        }
    }

    void EntitySignatureChanged(Entity entity, Signature signature) {
        // Notify each system of signature change
        for( auto const& pair : systems ) {
            auto const& key   = pair.first;
            auto const& system = pair.second;

            auto it = signatures.find( key );
            if(it == signatures.end() ) {
                // No signature for this system yet, skip
                continue;
            }
            const auto& system_sig = it->second;
            if( (signature & system_sig) == system_sig ) {
                system->entities.insert( entity );
            }
            else {
                system->entities.erase( entity );
            }
        }
    }

private:
    // Map: System Type (C String) -> Signature
    std::unordered_map<std::type_index, Signature> signatures{};

    // Map: System Type (C String) -> System Ptr
    std::unordered_map<std::type_index, std::shared_ptr<System>> systems{};

    // List of existing systems for in-order iteration
    // TODO: Refactor this to include order
    std::vector<std::shared_ptr<System>> order;
};

#endif //SYSTEMMANAGER_HPP
