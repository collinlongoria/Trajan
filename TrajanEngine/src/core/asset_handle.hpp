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

template<class T>
class AssetHandle {
public:
    AssetHandle() = default;
    AssetHandle(UUID id, T* ptr) : m_id(id), m_ptr(ptr) {}

    // Access operator overloads
    T*       operator->()       { return m_ptr; }
    const T* operator->() const { return m_ptr; }
    T&        operator*()       { return *m_ptr; }
    const  T& operator*() const { return *m_ptr; }

    [[nodiscard]] bool isValid() const { return m_ptr != nullptr; }
    [[nodiscard]] UUID id() const { return m_id; }

private:
    UUID m_id{};
    T*   m_ptr = nullptr;
};

#endif //ASSET_HANDLE_HPP
