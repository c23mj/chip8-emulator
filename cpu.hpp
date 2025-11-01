#pragma once

#include <array>
#include <vector>
#include <stack>
#include <cstdint>
#include "constants.hpp"
#include "sdl3/window.hpp" 

using byte = std::uint8_t;

class CPU {
public:
    CPU();

    // Boot the CPU with a program (vector of bytes)
    void boot(std::vector<byte>& program);

private:
    // Fetch next 2-byte opcode and advance PC
    std::uint16_t fetch();

    // Flattened display index helper
    std::size_t displayIndex(std::size_t x, std::size_t y);

    // Decode and execute one opcode
    void decodeAndExecute(std::uint16_t opcode);

    // Initialization helpers
    void loadFontToMemory();
    void loadProgram(std::vector<byte>& program);

private:
    std::array<byte, constants::MemorySize> memory{};
    std::array<byte, constants::DisplayWidth * constants::DisplayHeight> display{};
    std::array<byte, 16> registers{};
    Window window;
    std::uint16_t i{0};
    std::uint16_t pc{0};
    std::stack<std::uint16_t> stack;
};
