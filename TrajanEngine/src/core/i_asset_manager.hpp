/*
* File: i_asset_manager.hpp
* Project: Trajan
* Author: ${AUTHOR}
* Created on: 7/30/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#ifndef I_ASSET_MANAGER_HPP
#define I_ASSET_MANAGER_HPP

#include "asset_handle.hpp"
#include "uuid.hpp"

// Non-templated base
class IAssetManager {
public:
    virtual ~IAssetManager() = default;

    // Functions the engine calls generically on all managers
    virtual void CollectGarbage() = 0; // Free unreferenced entries
    virtual void UnloadAll()      = 0; // Shutdown function
};

// Typed interface (potential for reflection?)
template<class AssetT>
class IAssetManagerT : public IAssetManager {
public:
    using Handle = AssetHandle<AssetT>;

    virtual Handle loadFromGUID(UUID id) = 0;
    virtual Handle loadFromFile(const std::string& virtualPath) = 0;

    // Refcount used by AssetHandle
    virtual void addRef(UUID id) = 0;
    virtual void release(UUID id) = 0;
};

#endif //I_ASSET_MANAGER_HPP
