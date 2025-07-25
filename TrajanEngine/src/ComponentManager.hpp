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
    }
};

#endif //COMPONENTMANAGER_HPP
