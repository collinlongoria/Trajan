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

#ifndef SYSTEMMANAGER_HPP
#define SYSTEMMANAGER_HPP
#include <memory>
#include <unordered_map>

#include "Component.hpp"
#include "Log.hpp"
#include "System.hpp"

class SystemManager {
public:
    template<typename T>
    std::shared_ptr<T> RegisterSystem() {
        const char* type = typeid(T).name();

        if( systems.contains( type ) ) {
            Log::Error("Tried to register system type " + std::string(type) + " more than once!");
            return nullptr;
        }

        // Create and return pointer to the system
        auto system = std::make_shared<T>();
        systems.insert( { type, system } );
        return system;
    }

    template<typename T>
    void SetSignature(Signature signature) {
        const char* type = typeid(T).name();

        if( !systems.contains( type ) ) {
            Log::Error("System type " + std::string(type) + " not registered!");
            return;
        }

        // Set signature for the system
        signatures.insert( { type, signature } );
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
            auto const& type = pair.first;
            auto const& system = pair.second;
            auto const& system_sig = signatures.at( type );

            // Entity signature matches system -> insert into system's set
            if( ( signature & system_sig ) == system_sig ) {
                system->entities.insert( entity );
            }
            // Entity does not match -> erase from the set
            else {
                system->entities.erase( entity );
            }
        }
    }

private:
    // Map: System Type (C String) -> Signature
    std::unordered_map<const char*, Signature> signatures{};

    // Map: System Type (C String) -> System Ptr
    std::unordered_map<const char*, std::shared_ptr<System>> systems{};
};

#endif //SYSTEMMANAGER_HPP
