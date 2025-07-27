/*
* File: ComponentArray
* Project: Trajan
* Author: Collin Longoria
* Created on: 7/15/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#ifndef COMPONENTARRAY_HPP
#define COMPONENTARRAY_HPP

#include <array>
#include <unordered_map>

#include "TrajanEngine.hpp"

#include "Entity.hpp"
#include "Log.hpp"

class IComponentArray {
public:
    virtual ~IComponentArray() = default;
    virtual void EntityDestroyed(Entity entity) = 0;
};

template <typename T>
class ComponentArray : public IComponentArray {
public:
    void InsertData(Entity entity, T component) {
        if( entityToIndexMap.contains( entity ) ) {
            Log::Error("Attempted redundant add of component " + std::string(typeid(T).name()) + " to entity of ID: " + std::to_string(entity) );
            return;
        }

        // Insert entry at end and update map
        size_t index = size;
        entityToIndexMap[entity] = index;
        indexToEntityMap[index] = entity;
        componentArray[index] = component;
        size++;
    }

    void RemoveData(Entity entity) {
        if( !entityToIndexMap.contains( entity ) ) {
            Log::Warn("Tried to remove " + std::string(typeid(T).name()) + " from non-owning entity of ID " + std::to_string(entity) );
            return;
        }

        // Copy element at end into deleted element's place <- Goal is to maintain density
        size_t indexRemoved = entityToIndexMap[entity];
        size_t indexLast = size - 1;
        componentArray[indexRemoved] = componentArray[indexLast];

        // Update the map
        Entity entityLast = indexToEntityMap[indexLast];
        entityToIndexMap[entityLast] = indexRemoved;
        indexToEntityMap[indexRemoved] = entityLast;

        entityToIndexMap.erase(entity);
        indexToEntityMap.erase(indexLast);

        size--;
    }

    void EntityDestroyed(Entity entity) override {
        // This check seems redundant
        if( entityToIndexMap.contains( entity ) ) {
            RemoveData(entity);
        }
    }

private:
    std::array<T, MAX_ENTITIES> componentArray;
    std::unordered_map<Entity, size_t> entityToIndexMap;
    std::unordered_map<Entity, size_t> indexToEntityMap;
    size_t size;
};

#endif //COMPONENTARRAY_HPP
