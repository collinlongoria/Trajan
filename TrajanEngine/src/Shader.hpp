/*
* File: Shader.hpp
* Project: Trajan
* Author: ${AUTHOR}
* Created on: 7/24/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#ifndef SHADER_HPP
#define SHADER_HPP
#include <string>

class Shader {
public:
    std::string name;
    uint64_t rendererHandle = 0;
};

#endif //SHADER_HPP
