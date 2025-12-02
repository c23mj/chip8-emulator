// #include <SDL3/SDL.h>
// #include <array>
// #include <cstdint>
// #include <cstdio>
// #include "input.hpp" 

// static void DrawKeypad(SDL_Renderer* r, const std::array<uint8_t,16>& keys) {
//     int w = 0, h = 0;
//     SDL_GetRenderOutputSize(r, &w, &h);

//     // Layout
//     const int cols = 4, rows = 4;
//     const float pad = 8.0f;              // outer padding
//     const float gap = 8.0f;              // gap between cells
//     const float availW = (float)w - 2*pad - (cols-1)*gap;
//     const float availH = (float)h - 2*pad - (rows-1)*gap;
//     const float cell   = SDL_min(availW / cols, availH / rows);
//     const float totalW = cols*cell + (cols-1)*gap;
//     const float totalH = rows*cell + (rows-1)*gap;
//     const float offX   = (w - totalW) * 0.5f;
//     const float offY   = (h - totalH) * 0.5f;

//     // Colors
//     const SDL_Color bg       = { 20, 20, 24, 255 };
//     const SDL_Color inactive = { 42, 44, 52, 255 };
//     const SDL_Color active   = { 56,150,201,255 };
//     const SDL_Color border   = { 220,220,220,255 };

//     SDL_SetRenderDrawColor(r, bg.r, bg.g, bg.b, bg.a);
//     SDL_RenderClear(r);

//     // Visual order table (same mapping you used): 
//     // 1 2 3 C / 4 5 6 D / 7 8 9 E / A 0 B F
//     static const uint8_t vis[4][4] = {
//         {0x1, 0x2, 0x3, 0xC},
//         {0x4, 0x5, 0x6, 0xD},
//         {0x7, 0x8, 0x9, 0xE},
//         {0xA, 0x0, 0xB, 0xF},
//     };

//     SDL_FRect rect; rect.w = rect.h = cell;

//     for (int row = 0; row < rows; ++row) {
//         for (int col = 0; col < cols; ++col) {
//             const uint8_t idx = vis[row][col];
//             const bool on = keys[idx] != 0;

//             rect.x = offX + col * (cell + gap);
//             rect.y = offY + row * (cell + gap);

//             const SDL_Color fill = on ? active : inactive;
//             SDL_SetRenderDrawColor(r, fill.r, fill.g, fill.b, fill.a);
//             SDL_RenderFillRect(r, &rect);

//             SDL_SetRenderDrawColor(r, border.r, border.g, border.b, border.a);
//             SDL_RenderRect(r, &rect);
//         }
//     }

//     SDL_RenderPresent(r);
// }

// int main(int argc, char** argv) {
//     (void)argc; (void)argv;

//     if (!SDL_Init(SDL_INIT_VIDEO)) {
//         SDL_Log("SDL_Init failed: %s", SDL_GetError());
//         return 1;
//     }

//     SDL_Window*   win = nullptr;
//     SDL_Renderer* ren = nullptr;
//     if (!SDL_CreateWindowAndRenderer("Keypad (hold 1-4, QWER, ASDF, ZXCV)", 800, 600, 0,
//                                      &win, &ren)) {
//         SDL_Log("CreateWindowAndRenderer failed: %s", SDL_GetError());
//         SDL_Quit();
//         return 1;
//     }

//     InputHandler input;  // uses SDL_PollEvent + SDL_GetKeyboardState internally

//     bool running = true;
//     while (running) {
//         InputEvent ev = input.poll();  // drains events + snapshots keys

//         if (ev.quit) running = false;

//         if (ev.restart) {
//             SDL_Log("Restart requested (space held this frame).");
//             // do whatever "restart" means in your app
//         }

//         DrawKeypad(ren, ev.keypad_state);
//         // Optional: cap frame rate or add SDL_Delay(1) if you want
//     }

//     SDL_DestroyRenderer(ren);
//     SDL_DestroyWindow(win);
//     SDL_Quit();
//     return 0;
// }
