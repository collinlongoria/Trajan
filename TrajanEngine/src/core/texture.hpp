/*
* File: Texture.hpp
* Project: Trajan
* Author: ${AUTHOR}
* Created on: 7/24/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include <cstdint>

class Texture {
public:
    uint32_t width = 0, height = 0;
    uint64_t rendererHandle = 0;
};

#endif //TEXTURE_HPP
