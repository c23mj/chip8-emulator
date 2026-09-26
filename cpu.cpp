#include "cpu.hpp"
#include <vector>
#include <stack>
#include <thread>
#include <chrono> 
#include <random>
#include <span>
#include <algorithm>
using byte = uint8_t; // ensure unsigned

uint8_t randomByte() {
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<int> dist(0, 255);
    return static_cast<uint8_t>(dist(rng));
}

CPU::CPU()
: window("CHIP-8", (int)constants::DisplayWidth, (int)constants::DisplayHeight)
{
    loadFontToMemory();
}

void CPU::restart(std::vector<byte>& program){
    // Reset registers and memory
    memory.fill(0);
    registers.fill(0);
    display.fill(0);
    while (!stack.empty()) stack.pop();
    pc = constants::ProgramStart;
    i = 0;
    loadFontToMemory();
    loadProgram(program);
}
void CPU::boot(std::vector<byte>& program, unsigned instructions_per_second) {
    loadProgram(program);
    pc = constants::ProgramStart;

    using clock = std::chrono::steady_clock;
    const auto timer_interval = std::chrono::duration<double>(1.0 / 60.0);
    const auto cpu_interval = std::chrono::duration<double>(1.0 / instructions_per_second);
    const auto frame_interval = timer_interval;

    auto last_update = clock::now();
    auto cpu_elapsed = std::chrono::duration<double>::zero();
    auto timer_elapsed = std::chrono::duration<double>::zero();
    auto frame_elapsed = frame_interval;
    bool restart_held = false;
    while (window.alive()) {
        const auto now = clock::now();
        const auto elapsed = now - last_update;
        last_update = now;
        cpu_elapsed += elapsed;
        timer_elapsed += elapsed;
        frame_elapsed += elapsed;

        InputEvent ev = input.poll();
        keypad = ev.keypad_state; 
        if (ev.quit) break;
        if (ev.restart && !restart_held) {
            this->restart(program);
            delay_timer = 0;
            sound_timer = 0;
            cpu_elapsed = std::chrono::duration<double>::zero();
            timer_elapsed = std::chrono::duration<double>::zero();
            frame_elapsed = frame_interval;
        }
        restart_held = ev.restart;

        while (timer_elapsed >= timer_interval) {
            if (delay_timer > 0) --delay_timer;
            if (sound_timer > 0) --sound_timer;
            timer_elapsed -= timer_interval;
        }

        // Bound catch-up work after a long pause so input and rendering stay responsive.
        unsigned cycles = 0;
        while (cpu_elapsed >= cpu_interval && cycles < instructions_per_second / 10 + 1) {
            const uint16_t instruction = fetch();
            decodeAndExecute(instruction);
            cpu_elapsed -= cpu_interval;
            ++cycles;
        }
        if (cpu_elapsed >= cpu_interval) cpu_elapsed = std::chrono::duration<double>::zero();

        if (frame_elapsed >= frame_interval) {
            window.draw(display);
            frame_elapsed -= frame_interval;
            if (frame_elapsed >= frame_interval) frame_elapsed = std::chrono::duration<double>::zero();
        }

        const auto until_next = std::min({cpu_interval - cpu_elapsed,
                                          timer_interval - timer_elapsed,
                                          frame_interval - frame_elapsed});
        if (until_next > std::chrono::duration<double>::zero())
            std::this_thread::sleep_for(until_next);
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
                switch(opcode & 0x000F) {
                    case 0x0: {
                        registers[x] = registers[y];
                        break;
                    }
                    case 0x1: {
                        registers[x] |= registers[y];
                        break;
                    }
                    case 0x2: {
                        registers[x] &= registers[y];
                        break;
                    }
                    case 0x3: {
                        registers[x] ^= registers[y];
                        break;
                    }
                    // 4, 7
                    case 0x4: {
                        uint8_t sum = registers[x] + registers[y];
                        byte flag = sum < registers[x] ? 1 : 0;
                        registers[x] = sum;
                        registers[0xF] = flag;
                        break;
                    }
                    case 0x5: {
                        byte flag = registers[x] >= registers[y] ? 1 : 0;
                        registers[x] -= registers[y];
                        registers[0xF] = flag;
                        break;
                    }
                    case 0x6: {
                        byte flag = registers[x] & 1;
                        registers[x] >>= 1;
                        registers[0xF] = flag;
                        break;
                    }

                    case 0x7: {
                        byte flag = registers[y] >= registers[x] ? 1 : 0;
                        registers[x] = registers[y] - registers[x];
                        registers[0xF] = flag;
                        break;
                    }

                    
                    case 0xE: {
                        byte flag = (registers[x] & (1 << 7)) >> 7;
                        registers[x] <<= 1;
                        registers[0xF] = flag;
                        break;
                    }
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
                registers[x] = randomByte() & nn;
                break; 
            case 0xD000: {
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

                break;
            }
            case 0xE000: {
                switch (opcode & 0x00FF){
                    case 0x9E:
                        skipNextIf(keypad[registers[x]] != 0);
                        break;
                    case 0xA1:
                        skipNextIf(keypad[registers[x]] == 0);
                        break;
                    default:
                        unrecognized = true;
                        break;
                }
                break;
            }

            case 0xF000: {
                switch (opcode & 0XFF) {
                    case 0x07:
                        registers[x] = delay_timer;
                        break;
                    case 0x0A:
                        {
                            bool key_pressed = false;
                            for (size_t key = 0; key < keypad.size(); ++key) {
                                if (keypad[key] != 0) {
                                    registers[x] = static_cast<byte>(key);
                                    key_pressed = true;
                                    break;
                                }
                            }
                            if (!key_pressed) {
                                pc -= 2; // block/retry
                            }
                        }
                        break;
                    case 0x15:
                        delay_timer = registers[x];
                        break;
                    case 0x18:
                        sound_timer = registers[x];
                        break;
                    case 0x1E:
                        i += registers[x];
                        break;
                    case 0x29: {
                        uint16_t nibble = registers[x] & 0x0F;
                        i = constants::FontStart + (nibble * 5);
                        break;
                    }
                    case 0x33: {
                        uint16_t tmp = registers[x];
                        memory[i + 2] = tmp % 10;
                        tmp /= 10;
                        memory[i + 1] = tmp % 10;
                        tmp /= 10;
                        memory[i] = tmp % 10;
                        break;
                    }
                    case 0x55: {
                        for (size_t idx = 0; idx <= x; ++idx) {
                            memory[i + idx] = registers[idx];
                        }
                        break;
                    }
                    case 0x65: {
                        for (size_t idx = 0; idx <= x; ++idx) {
                            registers[idx] = memory[i + idx];
                        }
                        break;
                    }

                }
                break;
            }
            default:
                unrecognized = true;
                break;
        }
}

void CPU::loadFontToMemory(){
    std::copy(std::begin(constants::Font), std::end(constants::Font), memory.begin() + constants::FontStart);
}

void CPU::loadProgram(std::vector<byte>& program){
    std::copy(program.begin(), program.end(), memory.begin() + constants::ProgramStart);
}

void CPU::skipNextIf(bool cond){
    if(cond) {
        pc += 2;
    }
}
