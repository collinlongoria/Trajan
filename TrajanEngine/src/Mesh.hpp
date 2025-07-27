/*
* File: Mesh.hpp
* Project: Trajan
* Author: ${AUTHOR}
* Created on: 7/24/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#ifndef MESH_HPP
#define MESH_HPP
#include <cstdint>
#include <string>

class Mesh {
public:
    std::string name;
    uint32_t vertexCount = 0, indexCount = 0;
    uint64_t rendererHandle = 0;
};

#endif //MESH_HPP
