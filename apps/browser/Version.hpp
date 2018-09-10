//
// Created by koncord on 07.08.18.
//

#pragma once

#include <string>
#include <cstdint>

#define MAJOR_VERSION 1
#define MINOR_VERSION 0

uint64_t constexpr make_version(uint64_t major, uint64_t minor) { return (major << 32) + minor; }
uint64_t constexpr version() { return make_version(MAJOR_VERSION, MINOR_VERSION); }
inline std::string strVersion() noexcept { return std::to_string(MAJOR_VERSION) + "." + std::to_string(MINOR_VERSION); }

#define FIX_UNTIL(_version, message) static_assert(version() < _version, message)
