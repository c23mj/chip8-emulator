// main.cpp  (no SDL includes)
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <charconv>
#include <string_view>
#include "cpu.hpp"

using byte = uint8_t; // keep consistent with your project

int main(int argc, char** argv) {
    if (argc < 2 || argc > 3) {
        std::cerr << "usage: " << argv[0] << " <program.ch8> [instructions-per-second]\n";
        return 1;
    }

    unsigned instructions_per_second = 6000;
    if (argc == 3) {
        const std::string_view arg(argv[2]);
        const auto result = std::from_chars(arg.data(), arg.data() + arg.size(), instructions_per_second);
        if (result.ec != std::errc{} || result.ptr != arg.data() + arg.size() ||
            instructions_per_second == 0) {
            std::cerr << "instructions-per-second must be a positive integer\n";
            return 1;
        }
    }

    const char* path = argv[1];
    std::ifstream rom(path, std::ios::binary);
    if (!rom) {
        std::cerr << "failed to open ROM: " << path << "\n";
        return 1;
    }

    std::vector<byte> program(
        (std::istreambuf_iterator<char>(rom)),
        std::istreambuf_iterator<char>()
    );

    try {
        CPU cpu;            // CPU constructs/owns Window internally
        cpu.boot(program, instructions_per_second);
    } catch (const std::exception& e) {
        std::cerr << "fatal error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
