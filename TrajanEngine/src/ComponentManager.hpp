/*
* File: ComponentManager.hpp
* Project: Trajan
* Author: ${AUTHOR}
* Created on: 7/24/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#ifndef COMPONENTMANAGER_HPP
#define COMPONENTMANAGER_HPP
#include <unordered_map>

#include "Component.hpp"
#include "ComponentArray.hpp"

class ComponentManager {
public:
    template<typename T>
    void RegisterComponent() {
        const char* typeName = typeid(T).name();

        if( componentTypes.contains(typeName) ) {
            Log::Error("Attempted register of component type " + std::string(typeid(T).name()) + " more than once!");
            return;
        }

        // Add the type to the map
        componentTypes.insert( { typeName, nextComponentType } );

        // Create a component array ptr and add it to the map
        componentArrays.insert( { typeName, std::make_shared<ComponentArray<T>>() } );

        // Increment to the next available register
        ++nextComponentType;
    }

    template<typename T>
    ComponentType GetComponentType() {
        const char* typeName = typeid(T).name();

        if( !componentTypes.contains(typeName) ) {
            Log::Error("Component type " + std::string(typeid(T).name()) + " not registered before use!");
            return -1; // TODO: make this all use std::optional
        }

        // Return component type
        return componentTypes[typeName];
    }

    template<typename T>
    void AddComponent(Entity entity, T component) {
        // Add a component to the array for given entity
        GetComponentArray<T>()->InsertData(entity, component);
    }

    template<typename T>
    void RemoveComponent(Entity entity) {
        // Remove a component from the array for given entity
        GetComponentArray<T>()->RemoveData(entity);
    }

    template<typename T>
    T& GetComponent(Entity entity) {
        // Return a reference to a component from the array for an entity
        return GetComponentArray<T>()->GetData(entity);
    }

    void EntityDestroyed(Entity entity) {
        // Notify all arrays that an entity has been destroyed
        for(auto const& pair : componentArrays) {
            auto const& component = pair.second;
            component->EntityDestroyed(entity);
        }
    }

private:
    // Map from Component Name (C String) to Component Type (uint8_t)
    std::unordered_map<const char*, ComponentType> componentTypes {};

    // Map from Component Name (C String) to Component Array
    std::unordered_map<const char*, std::shared_ptr<IComponentArray>> componentArrays {};

    // Component Types to be assigned to the next registered component (Starts at 0)
    ComponentType nextComponentType{};

    // Helper: Retrieve statically cast pointer to the ComponentArray of type T
    template<typename T>
    std::shared_ptr<ComponentArray<T>> GetComponentArray() {
        const char* typeName = typeid(T).name();

        if( componentArrays.find(typeName) == componentArrays.end() ) {
            Log::Error("Component of type: " + std::string(typeid(T).name()) + " requested, but not registered!");
            return nullptr;
        }

        return std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeName]);
    }
};

#endif //COMPONENTMANAGER_HPP
