/*
* File: Orchestrator.hpp
* Project: Trajan
* Author: ${AUTHOR}
* Created on: 7/26/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#ifndef ORCHESTRATOR_HPP
#define ORCHESTRATOR_HPP
#include <memory>

#include "component_manager.hpp"
#include "entity_manager.hpp"
#include "system_manager.hpp"

class Orchestrator {
public:
    void Initialize() {
        // Create all managers
        entityManager = std::make_unique<EntityManager>();
        componentManager = std::make_unique<ComponentManager>();
        systemManager = std::make_unique<SystemManager>();
    }

    /*********************************
    *
    * Entity Functions:
    *
    *********************************/

    Entity CreateEntity() {
        return entityManager->CreateEntity().value();
    }

    void DestroyEntity(Entity entity) {
        entityManager->DestroyEntity( entity );

        componentManager->EntityDestroyed( entity );
        systemManager->EntityDestroyed( entity );
    }

    /*********************************
    *
    * Component Functions:
    *
    *********************************/

    template<typename T>
    void RegisterComponent() {
        componentManager->RegisterComponent<T>();
        Log::Message("Component type registered: " + std::string(typeid(T).name()));
    }

    template<typename T>
    void AddComponent(Entity entity, T component) {
        componentManager->AddComponent<T>( entity, component );

        auto signature = entityManager->GetSignature( entity ).value();
        signature.set( componentManager->GetComponentType<T>(), true );
        entityManager->SetSignature( entity, signature );

        systemManager->EntitySignatureChanged( entity, signature );
    }

    template<typename T>
    void RemoveComponent(Entity entity) {
        componentManager->RemoveComponent<T>( entity );

        auto signature = entityManager->GetSignature( entity ).value();
        signature.set( componentManager->GetComponentType<T>(), false );
        entityManager->SetSignature( entity, signature );

        systemManager->EntitySignatureChanged( entity, signature );
    }

    template<typename T>
    T& GetComponent(Entity entity) {
        return componentManager->GetComponent<T>( entity );
    }

    template<typename T>
    ComponentType GetComponentType() {
        return componentManager->GetComponentType<T>();
    }

    /*********************************
    *
    * System Functions:
    *
    *********************************/

    template<typename T>
    std::shared_ptr<T> RegisterSystem() {
        return systemManager->RegisterSystem<T>();
    }

    template<typename T>
    void SetSystemSignature(Signature signature) {
        systemManager->SetSignature<T>(signature);
    }

    void InitializeSystems(const SystemContext& ctx) {
        systemManager->InitializeSystems( ctx );
    }

    void UpdateSystems(float dt) {
        systemManager->UpdateSystems( dt );
    }

    void ShutdownSystems() {
        systemManager->ShutdownSystems();
    }

    // Helper function for registering systems
    template <typename SystemType, typename... ComponentTypes>
    void CreateSystem() {
        // Register the system to the orchestrator
        auto system = RegisterSystem<SystemType>();

        // Create signature
        Signature signature;
        (signature.set(GetComponentType<ComponentTypes>()), ...);

        // Set the signature in the system
        SetSystemSignature<SystemType>(signature);

        Log::Message("Registered system: " + std::string(typeid(SystemType).name()));
    }

private:
    std::unique_ptr<EntityManager> entityManager;
    std::unique_ptr<ComponentManager> componentManager;
    std::unique_ptr<SystemManager> systemManager;
    // TODO: event bus
};

#endif //ORCHESTRATOR_HPP
