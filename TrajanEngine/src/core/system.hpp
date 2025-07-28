/*
* File: System.hpp
* Project: Trajan
* Author: ${AUTHOR}
* Created on: 7/26/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#ifndef SYSTEM_HPP
#define SYSTEM_HPP
#include <set>

#include "entity.hpp"

class System {
public:
    std::set<Entity> entities;
};

#endif //SYSTEM_HPP
