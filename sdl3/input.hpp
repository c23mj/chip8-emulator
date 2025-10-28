#pragma once
#include <array>
#include <cstdint>
#include <SDL3/SDL.h>

struct InputEvent {
    bool quit;
    bool restart;
    std::array<uint8_t, 16> keypad_state;
};

class InputHandler {
public:
    explicit InputHandler() {
        // Nothing special here; caller must have called SDL_Init
    }

    InputEvent poll() {
        InputEvent out{};
        out.quit = false;
        out.restart = false;
        out.keypad_state.fill(0);

        // 1) Drain the SDL event queue
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                out.quit = true;
            }
        }

        // 2) Snapshot of keys currently held
        int numKeys = 0;
        const bool* keys = SDL_GetKeyboardState(&numKeys);

        auto held = [&](SDL_Scancode sc) -> bool {
            return (sc < numKeys) && keys[sc];
        };

        // --- CHIP-8 keypad mapping ---
        // Row 1
        if (held(SDL_SCANCODE_1)) out.keypad_state[0x1] = 1;
        if (held(SDL_SCANCODE_2)) out.keypad_state[0x2] = 1;
        if (held(SDL_SCANCODE_3)) out.keypad_state[0x3] = 1;
        if (held(SDL_SCANCODE_4)) out.keypad_state[0xC] = 1;
        // Row 2
        if (held(SDL_SCANCODE_Q)) out.keypad_state[0x4] = 1;
        if (held(SDL_SCANCODE_W)) out.keypad_state[0x5] = 1;
        if (held(SDL_SCANCODE_E)) out.keypad_state[0x6] = 1;
        if (held(SDL_SCANCODE_R)) out.keypad_state[0xD] = 1;
        // Row 3
        if (held(SDL_SCANCODE_A)) out.keypad_state[0x7] = 1;
        if (held(SDL_SCANCODE_S)) out.keypad_state[0x8] = 1;
        if (held(SDL_SCANCODE_D)) out.keypad_state[0x9] = 1;
        if (held(SDL_SCANCODE_F)) out.keypad_state[0xE] = 1;
        // Row 4
        if (held(SDL_SCANCODE_Z)) out.keypad_state[0xA] = 1;
        if (held(SDL_SCANCODE_X)) out.keypad_state[0x0] = 1;
        if (held(SDL_SCANCODE_C)) out.keypad_state[0xB] = 1;
        if (held(SDL_SCANCODE_V)) out.keypad_state[0xF] = 1;

        // Extra controls
        if (held(SDL_SCANCODE_ESCAPE)) out.quit = true;
        if (held(SDL_SCANCODE_SPACE))  out.restart = true;

        return out;
    }
};
