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
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " <program.ch8> [instructions-per-second] [--profile]\n";
        return 1;
    }

    unsigned instructions_per_second = 6000;
    bool rate_set = false;
    bool profile = false;
    for (int arg_index = 2; arg_index < argc; ++arg_index) {
        const std::string_view arg(argv[arg_index]);
        if (arg == "--profile" && !profile) {
            profile = true;
        } else if (!rate_set) {
            const auto result = std::from_chars(arg.data(), arg.data() + arg.size(), instructions_per_second);
            if (result.ec == std::errc{} && result.ptr == arg.data() + arg.size() &&
                instructions_per_second > 0) {
                rate_set = true;
                continue;
            }
            std::cerr << "instructions-per-second must be a positive integer\n";
            return 1;
        } else {
            std::cerr << "usage: " << argv[0]
                      << " <program.ch8> [instructions-per-second] [--profile]\n";
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
        cpu.boot(program, instructions_per_second, profile);
    } catch (const std::exception& e) {
        std::cerr << "fatal error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
