#include "cpu.hpp"
#include <vector>
#include <stack>
#include <thread>
#include <chrono> 
#include <span>
using byte = uint8_t; // ensure unsigned

CPU::CPU()
: window("CHIP-8", (int)constants::DisplayWidth, (int)constants::DisplayHeight) {
    loadFontToMemory();
}

void CPU::boot(std::vector<byte>& program){
    loadProgram(program);
    pc = constants::ProgramStart;
    while (window.alive()) {  // optional: exit when user quits
        const uint16_t instruction = fetch();
        decodeAndExecute(instruction);
        std::this_thread::sleep_for(std::chrono::nanoseconds(1'000'000'000 / 700));
    }
}

std::uint16_t CPU::fetch(){
    uint16_t opcode = memory[pc] << 8 | memory[pc + 1];
    pc += 2;
    return opcode;
}

std::size_t CPU::displayIndex(std::size_t x, std::size_t y){
    return y * constants::DisplayWidth + x;
}

void CPU::decodeAndExecute(uint16_t opcode){
        byte x = (opcode & 0x0F00) >> 8;
        byte y = (opcode & 0x00F0) >> 4;
        byte n = opcode & 0x000F;
        byte nn = opcode & 0x00FF;
        uint16_t nnn = opcode & 0x0FFF;
        bool unrecognized = false;


        switch (opcode & 0xF000){
            case 0x0000:
                switch (opcode){
                    case 0x00E0:
                        display.fill(0);
                        break;
                    case 0x00EE:
                        pc = stack.top();
                        stack.pop();
                        break;
                    default:
                        unrecognized = true;
                        break;
                }
                break;
            case 0x1000:
                pc = nnn;
                break;
            case 0x2000:
                stack.push(pc);
                pc = nnn;
                break;
            case 0x3000:
                skipNextIf(registers[x] == nn);
                break;
            case 0x4000:
                skipNextIf(registers[x] != nn);
                break;
            case 0x5000:
                skipNextIf(registers[x] == registers[y]);
                break;
            case 0x6000:
                registers[x] = nn;
                break;
            case 0x7000:
                registers[x] += nn;
                break;
            case 0x8000:
                switch(opcode && 0x000F) {
                    case 0x0:
                        registers[x] = registers[y];
                        break;
                    case 0x1:
                        registers[x] |= registers[y];
                        break;
                    case 0x2:
                        registers[x] &= registers[y];
                        break;
                    case 0x3:
                        registers[x] ^= registers[y];
                        break;
                    case 0x4:  
                        uint8_t sum = registers[x] + registers[y];
                        if (sum < registers[x]) registers[0xF] = 1; // overflow
                        registers[x] = sum;
                        break;
                    case 0x5:
                        if (registers[x] > registers[y]) registers[0xF] = 1;
                        registers[x] -= registers[y];
                        break;
                    case 0x6:
                        registers[0xF] = registers[x] & 1;
                        registers[x] >>= 1;
                        break;

                    case 0x7:
                        if (registers[y] > registers[x]) registers[0xF] = 1;
                        registers[y] -= registers[x];
                        break;
                    
                    case 0xE:
                        registers[0xF] = (registers[x] & (1 << 7)) >> 7;
                        registers[x] >>= 1;
                        break;
                }
                break;
            case 0x9000:
                skipNextIf(registers[x] != registers[y]);
                break;
            case 0xA000:
                i = nnn;
                break;
            case 0xB000:
                pc = nnn + registers[0];
                break;
            case 0xC000: 
                break; 
            case 0xD000:
                const uint8_t vx = registers[x] & 63;
                const uint8_t vy = registers[y] & 31;
                registers[0xF] = 0;

                std::span<const byte> sprite{ memory.data() + i, static_cast<size_t>(n) };

                size_t py = vy;
                for (byte sb : sprite) {
                    uint8_t mask = 1 << 7;
                    for (size_t idx = 0; idx < 8; ++idx, mask >>= 1) {
                        if (sb & mask) {
                            const std::size_t px = (vx + idx);
                            if (px >= constants::DisplayWidth) break;
                            const auto di = displayIndex(px, py);
                            if (display[di] == 1) {
                                registers[0xF] = 1; // collision
                            }
                            display[di] ^= 1;
                        }
                    }
                    if (++py >= constants::DisplayHeight) break;
                }

                window.draw(display);
                break;
            // case 0xE000:
            //     break;
            // case 0xF000:
            //     break;
        }
}

void CPU::loadFontToMemory(){
    std::copy(std::begin(constants::Font), std::end(constants::Font), memory.begin() + 0x50);
}

void CPU::loadProgram(std::vector<byte>& program){
    std::copy(program.begin(), program.end(), memory.begin() + constants::ProgramStart);
}

void CPU::skipNextIf(bool cond){
    if(cond) {
        pc += 2;
    }
}

