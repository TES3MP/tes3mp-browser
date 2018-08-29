//
// Created by koncord on 07.08.18.
//

#pragma once

#include <string>

#define MAJOR_VERSION 1
#define MINOR_VERSION 0

unsigned long constexpr make_version(unsigned long major, unsigned long minor) { return (major << 32) + minor; }
unsigned long constexpr version() { return make_version(MAJOR_VERSION, MINOR_VERSION); }
inline std::string strVersion() noexcept { return std::to_string(MAJOR_VERSION) + "." + std::to_string(MINOR_VERSION); }

#define FIX_UNTIL(_version, message) static_assert(version() < _version, message)
