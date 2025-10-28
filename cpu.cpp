#include <vector>
#include <stack>
#include <thread>
#include <chrono> 
#include <span>
#include "constants.hpp"

using byte = uint8_t;
class CPU{
public:
    CPU(){
        loadFontToMemory();
    }

    void boot(std::vector<byte>& program){
        loadProgram(program);
        pc = constants::ProgramStart;
        while(true){
            uint16_t instruction = fetch();
            decodeAndExecute(instruction);
            std::this_thread::sleep_for(std::chrono::nanoseconds(1'000'000'000 / 700));
        }
    }



private:
    std::array<byte, constants::MemorySize> memory{};
    std::array<uint8_t, constants::DisplayWidth * constants::DisplayHeight> display{};
    std::array<byte, 16> registers{};
    uint16_t i;
    uint16_t pc;
    std::stack<uint16_t> stack;

    uint16_t fetch(){
        uint16_t opcode = memory[pc] << 8 | memory[pc + 1];
        pc += 2;
        return opcode;
    }
    size_t displayIndex(size_t x, size_t y){
        return y * constants::DisplayWidth + x;
    }
    void decodeAndExecute(uint16_t opcode){
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
                break;
            case 0x3000:
                break;
            case 0x4000:
                break;
            case 0x5000:
                break;
            case 0x6000:
                registers[x] = nn;
                break;
            case 0x7000:
                registers[x] += nn;
                break;
            case 0x8000:
                break;
            case 0x9000:
                break;
            case 0xA000:
                i = nnn;
                break;
            case 0xB000:
                break;
            case 0xC000: 
                break; 
            case 0xD000:
                registers[x] &= 63;
                registers[y] &= 31;
                registers[0xF] = 0;
                std::span<const byte> sprite{ memory.data() + i, static_cast<size_t>(n) };
                
                byte mask = 1 << 7;
                for (auto byte : sprite) {
                    if (y >= constants::DisplayHeight) break;
                    for (size_t idx = 0; idx < 8; idx++) {
                        if (x + idx >= constants::DisplayWidth) break;
                        if (byte & mask) {
                            if (display[displayIndex(x + idx, y)] == 1) {
                                registers[0xF] = 1;
                            }
                            display[displayIndex(x + idx, y)] ^= 1;
                        }
                        mask >>= 1;
                    }
                    y++;
                }      
                break;
            case 0xE000:
                break;
            case 0xF000:
                break;
        }
    } 
  
    void loadFontToMemory(){
        std::copy(std::begin(constants::Font), std::end(constants::Font), memory.begin() + 0x50);
    }
    void loadProgram(std::vector<byte>& program){
        std::copy(program.begin(), program.end(), memory.begin() + constants::ProgramStart);
    }
};

