/*
* File: ILogger
* Project: Trajan
* Author: Collin Longoria
* Created on: 7/15/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#ifndef I_LOGGER_HPP
#define I_LOGGER_HPP

#include <string>
#include <memory>

class ILogger {
public:
    virtual ~ILogger() = default;

    virtual void Message(const std::string& message) = 0;
    virtual void Warn(const std::string& message) = 0;
    virtual void Error(const std::string& message) = 0;

    virtual void Assert(bool condition, const std::string& message) = 0;
};

#endif //I_LOGGER_HPP
