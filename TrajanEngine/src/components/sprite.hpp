/*
* File: sprite.hpp
* Project: Trajan
* Author: ${AUTHOR}
* Created on: 8/1/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#ifndef SPRITE_HPP
#define SPRITE_HPP

#include "mesh.hpp"
#include "shader.hpp"

struct Sprite {
    Mesh* mesh = nullptr;
    Shader* shader = nullptr;
};

#endif //SPRITE_HPP
