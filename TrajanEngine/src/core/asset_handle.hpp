/*
* File: asset_handle.hpp
* Project: Trajan
* Author: ${AUTHOR}
* Created on: 7/30/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#ifndef ASSET_HANDLE_HPP
#define ASSET_HANDLE_HPP

#include "uuid.hpp"

// Forward declare typed manager to avoid circular includes
template<class T>
class IAssetManagerT;

template<class T>
class AssetHandle {
public:
    AssetHandle() = default;

    // Legacy constructor (no manager)
    // Does not handle lifetime
    [[deprecated]] AssetHandle(UUID id, T* ptr) : m_id(id), m_ptr(ptr), m_mgr(nullptr) {}

    // Correct constructor. Handles lifetime.
    AssetHandle(UUID id, T* ptr, IAssetManagerT<T>* mgr, bool addRef = true)
        : m_id(id), m_ptr(ptr), m_mgr(mgr)
    {
        if ( addRef && m_mgr ) m_mgr->addRef(m_id);
    }

    // Copy: bump ref
    AssetHandle(const AssetHandle& other)
        : m_id(other.m_id), m_ptr(other.m_ptr), m_mgr(other.m_mgr)
    {
        if ( m_mgr ) m_mgr->addRef(m_id);
    }

    // Move: steal
    AssetHandle(AssetHandle&& other) noexcept
        : m_id(other.m_id), m_ptr(other.m_ptr), m_mgr(other.m_mgr)
    {
        other.m_id = {};
        other.m_ptr = nullptr;
        other.m_mgr = nullptr;
    }

    AssetHandle& operator=(const AssetHandle& other) noexcept {
        if(this == &other) return *this;
        if(other.m_mgr) other.m_mgr->addRef(other.m_id);
        reset();
        m_id = other.m_id;
        m_ptr = other.m_ptr;
        m_mgr = other.m_mgr;
        return *this;
    }

    ~AssetHandle() {
        reset();
    }

    void reset() {
        if ( m_mgr ) m_mgr->release(m_id);
        m_id = {};
        m_ptr = nullptr;
        m_mgr = nullptr;
    }

    void swap(AssetHandle& other) noexcept {
        std::swap(m_id, other.m_id);
        std::swap(m_ptr, other.m_ptr);
        std::swap(m_mgr, other.m_mgr);
    }

    // Access operator overloads
    T*       operator->()       { return m_ptr; }
    const T* operator->() const { return m_ptr; }
    T&        operator*()       { return *m_ptr; }
    const  T& operator*() const { return *m_ptr; }

    [[nodiscard]] bool isValid() const { return m_ptr != nullptr; }
    [[nodiscard]] UUID      id() const { return m_id; }
    [[nodiscard]] T*       get() const { return m_ptr; }

private:
    UUID m_id{};
    T*   m_ptr = nullptr;
    IAssetManagerT<T>* m_mgr = nullptr;
};

#endif //ASSET_HANDLE_HPP
