// main.cpp  (no SDL includes)
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include "cpu.hpp"

using byte = uint8_t; // keep consistent with your project

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " <program.ch8>\n";
        return 1;
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
        cpu.boot(program);  // runs until user quits window or your loop stops
    } catch (const std::exception& e) {
        std::cerr << "fatal error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
