/*
* File: Log
* Project: Trajan
* Author: Collin Longoria
* Created on: 7/15/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#ifndef LOG_HPP
#define LOG_HPP

#include "TrajanEngine.hpp"

namespace Log {
    inline void Message(const std::string& message) {
        Trajan::GetLogger()->Message(message);
    }

    inline void Warn(const std::string& message) {
        Trajan::GetLogger()->Warn(message);
    }

    inline void Error(const std::string& message) {
        Trajan::GetLogger()->Error(message);
    }

    inline void Assert(bool condition, const std::string& message) {
        Trajan::GetLogger()->Assert(condition, message);
    }
}

#endif //LOG_HPP
