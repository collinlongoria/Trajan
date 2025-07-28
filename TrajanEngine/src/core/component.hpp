/*
* File: Component
* Project: Trajan
* Author: Collin Longoria
* Created on: 7/14/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include <cstdint>
#include <bitset>

// Component type alias
using ComponentType = uint8_t;

// Maximum number of components
const ComponentType MAX_COMPONENTS = 32;

// Bitset of component types
using Signature = std::bitset<MAX_COMPONENTS>;

#endif //COMPONENT_HPP
