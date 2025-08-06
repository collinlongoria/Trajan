/*
* File: transform_2d.hpp
* Project: Trajan
* Author: ${AUTHOR}
* Created on: 8/1/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#ifndef TRANSFORM_2D_HPP
#define TRANSFORM_2D_HPP
#include "math.hpp"

struct Transform2D {
    Vector2 position;
    float rotation;
    Vector2 scale;

    [[nodiscard]] Matrix4 Matrix() const {
        Matrix4 m(1.0f);

        m = glm::translate(m, Vector3(position, 0.f));
        m = glm::rotate(m, rotation, Vector3(0.f, 0.f, 1.f));
        m = glm::scale(m, Vector3(scale, 1.f));

        return m;
    }
};

#endif //TRANSFORM_2D_HPP
