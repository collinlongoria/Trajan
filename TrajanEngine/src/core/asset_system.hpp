/*
* File: AssetSystem.hpp
* Project: Trajan
* Author: ${AUTHOR}
* Created on: 8/1/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#ifndef ASSETSYSTEM_HPP
#define ASSETSYSTEM_HPP
#include <memory>
#include <type_traits>
#include <vector>

#include "i_asset_manager.hpp"

class AssetSystem {
public:
    template<class ManagerT, class... Args>
    ManagerT& EmplaceManager(Args&&... args) {
        //Log::Assert(std::is_base_of_v<IAssetManager, ManagerT>, "ManagerT must be derived from IAssetManager");

        auto mgr = std::make_unique<ManagerT>(std::forward<Args>(args)...);
        ManagerT& ref = *mgr;
        m_managers.emplace_back(std::move(mgr));
        return ref;
    }

    template<class ManagerT>
    ManagerT& Get() {
        for(auto& m : m_managers) {
            if(auto p = dynamic_cast<ManagerT*>(m.get())) return *p;
        }

        //Log::Error("Requested Manager does not exist"); // TODO: add Log::Throw
        throw std::runtime_error("Get() failed");
    }

    void CollectGarbage() {
        for (auto& mgr : m_managers) mgr->CollectGarbage();
    }

    void UnloadAssets() {
        for(auto& mgr : m_managers) mgr->UnloadAll();
    }

private:
    std::vector<std::unique_ptr<IAssetManager>> m_managers;
};

#endif //ASSETSYSTEM_HPP
