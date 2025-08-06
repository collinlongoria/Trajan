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

#ifndef COMPONENT_MANAGER_HPP
#define COMPONENT_MANAGER_HPP
#include <unordered_map>
#include <typeindex>

#include "component.hpp"
#include "component_array.hpp"

class ComponentManager {
public:
    template<typename T>
    void RegisterComponent() {
        auto key = std::type_index(typeid(T));

        if( componentTypes.contains(key) ) {
            Log::Error("Attempted register of component type " + std::string(typeid(T).name()) + " more than once!");
            return;
        }

        // Add the type to the map
        componentTypes.insert( { key, nextComponentType } );

        // Create a component array ptr and add it to the map
        componentArrays.insert( { key, std::make_shared<ComponentArray<T>>() } );

        // Increment to the next available register
        ++nextComponentType;
    }

    template<typename T>
    ComponentType GetComponentType() {
        auto key = std::type_index(typeid(T));
        auto it = componentTypes.find( key );
        if(it == componentTypes.end()) {
            Log::Assert(false, "Component type not registered"); // TODO: replace when i add throws
        }
        return it->second;
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
        auto arr = GetComponentArray<T>();
        Log::Assert(arr != nullptr, "Component array does not exist");
        T* ptr = arr->GetData(entity);
        Log::Assert(ptr != nullptr, "Component not found on entity");
        return *ptr;
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
    std::unordered_map<std::type_index, ComponentType> componentTypes {};

    // Map from Component Name (C String) to Component Array
    std::unordered_map<std::type_index, std::shared_ptr<IComponentArray>> componentArrays {};

    // Component Types to be assigned to the next registered component (Starts at 0)
    ComponentType nextComponentType{};

    // Helper: Retrieve statically cast pointer to the ComponentArray of type T
    template<typename T>
    std::shared_ptr<ComponentArray<T>> GetComponentArray() {
        auto key = std::type_index(typeid(T));
        auto it = componentArrays.find( key );
        if(it == componentArrays.end()) {
            Log::Error("Component of type: " + std::string(typeid(T).name()) + " requested, but not registered.");
            return nullptr;
        }
        return std::static_pointer_cast<ComponentArray<T>>(it->second);
    }
};

#endif //COMPONENT_MANAGER_HPP
