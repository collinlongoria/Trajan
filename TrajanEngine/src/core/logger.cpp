/*
* File: Logger
* Project: Trajan
* Author: Collin Longoria
* Created on: 7/15/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#include "logger.hpp"

#include <iostream>

// Helper function to output to file/console
void Logger::Output(const std::string& str) {
    // TODO: Change this to handle turning off console when not in debug
    std::cout << str << "\n";

    // Write to log file
    if(file.is_open()) {
        file << str << "\033[0;37m\n";
    }
}

Logger::Logger() {
    // Open the log file
    file.open("Log.txt", std::ofstream::out | std::ofstream::trunc);
    if(!file.is_open()) {
        std::cerr << "TRAJAN failed to open Log file." << std::endl;
    }

    // Retrieve starting timestamp
    start_time = std::chrono::steady_clock::now();
}

Logger::~Logger() {
    // Reset console (could be needed, but could also not be)
    std::cout << "\033[0;37m\n";

    if(file.is_open()) {
        file.close();
    }
}

void Logger::Message(const std::string &message) {
    // Let only one thread write at a time
    std::lock_guard<std::mutex> lock(mutex);

    // Get timestamp
    const std::string timestamp_str = GetTime();

    std::string color_str = "\033[0;37m";
    std::string log_str = "[TRAJAN][" + timestamp_str + "][MESSAGE] " + message;

    Output(color_str + log_str);
}

void Logger::Warn(const std::string &message) {
    // Let only one thread write at a time
    std::lock_guard<std::mutex> lock(mutex);

    // Get timestamp
    const std::string timestamp_str = GetTime();

    std::string color_str = "\033[0;33m";
    std::string log_str = "[TRAJAN][" + timestamp_str + "][WARNING] " + message;

    Output(color_str + log_str);
}

void Logger::Error(const std::string &message) {
    // Let only one thread write at a time
    std::lock_guard<std::mutex> lock(mutex);

    // Get timestamp
    const std::string timestamp_str = GetTime();

    std::string color_str = "\033[0;31m";
    std::string log_str = "[TRAJAN][" + timestamp_str + "][ERROR  ] " + message;

    //Output(color_str + log_str)

    // override Output() so I can call cerr (maybe do this better?)
    std::cerr << color_str << log_str << "\n";

    // Write to log file
    if(file.is_open()) {
        file << color_str << log_str << "\033[0;37m\n";
    }
}

void Logger::Assert(bool condition, const std::string &message) {
    // Let only one thread write at a time
    std::lock_guard<std::mutex> lock(mutex);

    // Check condition, shutdown if true
    if(!condition) {
        // Get timestamp
        const std::string timestamp_str = GetTime();

        std::string color_str = "\033[41;30m";
        std::string log_str = "[TRAJAN][" + timestamp_str + "][ASSERT ] " + message;

        Output(color_str + log_str);

        // TODO: add engine force shutdown
    }
}

std::string Logger::GetTime() {
    using namespace std::chrono;

    auto now = system_clock::now();
    std::time_t now_t = system_clock::to_time_t(now);

    std::tm local_time;
#ifdef _WIN32
    localtime_s(&local_time, &now_t);
#else
    localtime_r(&now_t, &local_time);
#endif

    // (format is HH::MM::SS)
    char buffer[9];
    std::strftime(buffer, sizeof(buffer), "%H:%M:%S", &local_time);

    // calculate elapsed time
    // (format is HH::MM::SS.mmm)
    auto elapsed = duration_cast<milliseconds>(steady_clock::now() - start_time);
    int elapsed_hours = static_cast<int>(elapsed.count()) / 3600000;
    int elapsed_minutes = (elapsed.count() % 3600000) / 60000;
    int elapsed_seconds = (elapsed.count() % 60000) / 1000;
    int elapsed_milliseconds = elapsed.count() % 1000;

    // Format elapsed time
    char elapsed_buffer[16];
    std::snprintf(elapsed_buffer, sizeof(elapsed_buffer), "%02d:%02d:%02d.%03d", elapsed_hours, elapsed_minutes, elapsed_seconds, elapsed_milliseconds);

    return std::string(buffer) + "|" + elapsed_buffer;
}