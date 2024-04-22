#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include "./object.h"

namespace monkey {
    // define types
    using byte = uint8_t;
    using Instructions = std::vector<byte>;
    using Opcode = byte;
    using offset_t = byte;
    using width_t = byte;

    using Constants = std::vector<std::shared_ptr<Object>>;
    using Stack = std::vector<std::shared_ptr<Object>>;
    using Globals = std::vector<std::shared_ptr<Object>>;
}