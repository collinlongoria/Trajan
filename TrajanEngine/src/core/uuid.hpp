/*
* File: uuid.hpp
* Project: Trajan
* Author: ${AUTHOR}
* Created on: 7/30/2025
*
* Copyright (c) 2025 Collin Longoria
*
* This software is released under the MIT License.
* https://opensource.org/licenses/MIT
*/

#ifndef UUID_HPP
#define UUID_HPP
#include <array>
#include <cstdint>
#include <iomanip>
#include <string>
#include <sstream>
#include <random>
#include <cstring>

#include "log.hpp"

struct UUID {
    // Simple 128-bit RFC 4122-style UUID
    // Defaults to 0 = null
    std::array<uint8_t, 16> bytes{};

    /*
     *  Function: generate
     *
     *  Description:
     *      Factory that creates a random version-4 UUID
     *
     *  In:
     *      none
     *
     *  Out:
     *      UUID - the new generated UUID
     */
    static UUID generate() {
        static thread_local std::mt19937_64 rng{ std::random_device{}() };
        UUID id;
        for(auto& byte : id.bytes) byte = static_cast<uint8_t>( rng() );

        // Set variant (RFC 4122 -> 10xxxxxx) and version (4 = 0100xxxx)
        id.bytes[6] = ( id.bytes[6] & 0x0F ) | 0x40;
        id.bytes[8] = ( id.bytes[8] & 0x3F ) | 0x80;

        return id;
    }

    /*
     *  Function: fromString
     *
     *  Description:
     *      Parses a string into a UUID
     *
     *  In:
     *      std::string_view str - the string to be parsed
     *
     *  Out:
     *      UUID - the UUID derived from the parsed string
     */
    static UUID fromString(std::string_view str) {
        UUID id;
        std::string hex;
        hex.reserve(32);
        for(char c : str) {
            if(std::isxdigit(static_cast<unsigned char>(c))) {
                hex.push_back(c);
            }
        }

        if(hex.size() != 32) {
            Log::Error("Malformed UUID");
        }

        for(std::size_t i = 0; i < 16; ++i) {
            id.bytes[i] = static_cast<uint8_t>( std::stoul( hex.substr( i*2, 2 ), nullptr, 16 ) );
        }

        return id;
    }

    /*
     *  Function: toString
     *
     *  Description:
     *      Converts UUID into a std::string
     *
     *  In:
     *      none
     *
     *  Out:
     *      std::string of the UUID
     */
    [[nodiscard]] std::string toString() const {
        std::ostringstream ss;
        ss << std::hex << std::setfill('0');
        const int groups[5] = { 4, 2, 2, 2, 6 };
        int idx = 0;
        for(int g = 0; g < 5; ++g) {
            for(int i = 0; i < groups[g]; ++i) {
                ss << std::setw(2) << static_cast<int>(bytes[idx++]);
            }
            if(g != 4) {
                ss << '-';
            }
        }
        return ss.str();
    }

    // Comparison overloads
    friend bool operator==(const UUID& lhs, const UUID& rhs) { return lhs.bytes == rhs.bytes; }
    friend bool operator!=(const UUID& lhs, const UUID& rhs) { return !(lhs == rhs); }

    struct Hasher {
        std::size_t operator()(const UUID& id) const noexcept {
            // Simply using first 8 bytes as 64-bit hash
            uint64_t h = 0;
            std::memcpy(&h, id.bytes.data(), sizeof(uint64_t));
            return std::hash<uint64_t>{}(h);
        }
    };
};

#endif //UUID_HPP
