/*
* File: Logger
* Project: Trajan
* Author: colli
* Created on: 7/15/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "trajan_engine.hpp"
#include <fstream>
#include <mutex>
#include <chrono>

class Logger : public ILogger {
public:
    Logger();
    ~Logger() override;

    void Message(const std::string& message) override;

    void Warn(const std::string& message) override;

    void Error(const std::string& message) override;

    void Assert(bool condition, const std::string& message) override;

    // Delete, copy, and move constructors/assignments
    Logger(const Logger&)            = delete;
    Logger(Logger&&)                 = delete;
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&)      = delete;

private:
    /*
     *  Function: GetTime
     *
     *  Description:
     *      Returns the current timestamp
     *
     *  In:
     *      none
     *
     *  Out:
     *      std::string of current timestamp
     */
    std::string GetTime();

    /*
     *  Function: Output
     *
     *  Description:
     *      Helper function for outputting to File and Console
     *
     *  In:
     *      str - std::string message to be output
     *
     *  Out:
     *      none
     */
    void Output(const std::string& str);

    // Output file stream
    std::ofstream file;

    // Ensures thread safe writes
    std::mutex mutex;

    // Start time
    std::chrono::steady_clock::time_point start_time;
};

/*
 *  Function: GetLogger
 *
 *  Description:
 *      Returns the singelton instance of Logger
 *
 *  In:
 *      none
 *
 *  Out:
 *      std::shared_ptr of Logger instance
 */
inline std::shared_ptr<ILogger> Trajan::GetLogger() {
    static std::shared_ptr<Logger> instance = std::make_shared<Logger>();
    return instance;
}


#endif //LOGGER_HPP
