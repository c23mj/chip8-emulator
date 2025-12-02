#pragma once

#include <array>
#include <vector>
#include <stack>
#include <cstdint>
#include "constants.hpp"
#include "sdl3/window.hpp" 
#include "sdl3/input.hpp"

using byte = std::uint8_t;

class CPU {
public:
    CPU();

    // Boot the CPU with a program (vector of bytes)
    void boot(std::vector<byte>& program);

private:
    // Fetch next 2-byte opcode, advance PC
    std::uint16_t fetch();

    // sends (x, y) coord to an index within the flattened display arr.
    std::size_t displayIndex(std::size_t x, std::size_t y);

    // Decode and execute one opcode
    void decodeAndExecute(std::uint16_t opcode);

    // skips to next instr. if cond is true
    void skipNextIf(bool cond);

    // Initialization helpers
    void loadFontToMemory();
    void loadProgram(std::vector<byte>& program);

private:
    void restart(std::vector<byte>& program);
    std::array<byte, constants::MemorySize> memory{};
    std::array<byte, constants::DisplayWidth * constants::DisplayHeight> display{};
    std::array<byte, 16> registers{};
    std::array<uint8_t, 16> keypad;
    Window window;
    InputHandler input;
    std::uint8_t delay_timer{0};   
    std::uint8_t sound_timer{0};
    std::uint16_t i{0};
    std::uint16_t pc{0};
    std::stack<std::uint16_t> stack;
};
