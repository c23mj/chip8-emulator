// /* checkerboard.c - Single-file SDL3 demo that auto-scales a 64x32 checkerboard.
//    Build notes:
//      - Define/Link SDL3 per your setup (e.g., xcframework on macOS, or pkg-config on other OSes).
//      - Uses SDL's callback entrypoints (SDL_MAIN_USE_CALLBACKS).
// */
// #define SDL_MAIN_USE_CALLBACKS 1
// #include <SDL3/SDL.h>
// #include <SDL3/SDL_main.h>
// #include "input.hpp"

// static SDL_Window*   gWindow   = NULL;
// static SDL_Renderer* gRenderer = NULL;
// static InputHandler  gInputHandler;

// enum { CHIP_W = 64, CHIP_H = 32 };  /* logical pixel grid (CHIP-8 size) */

// /* Runs once at startup. */
// SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
// {
//     (void)appstate; (void)argc; (void)argv;

//     SDL_SetAppMetadata("Auto-Scaled Checkerboard", "1.0", "com.example.checkerboard");

//     if (!SDL_Init(SDL_INIT_VIDEO)) {
//         SDL_Log("SDL_Init failed: %s", SDL_GetError());
//         return SDL_APP_FAILURE;
//     }

//     if (!SDL_CreateWindowAndRenderer("Checkerboard (Auto-Scaled 64x32)", 800, 600, 0,
//                                      &gWindow, &gRenderer)) {
//         SDL_Log("CreateWindowAndRenderer failed: %s", SDL_GetError());
//         return SDL_APP_FAILURE;
//     }

//     return SDL_APP_CONTINUE;
// }

// /* Handle events (quit, keyboard input, etc.). */
// SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
// {
//     (void)appstate;
    
//     // Handle quit event
//     if (event->type == SDL_EVENT_QUIT) {
//         return SDL_APP_SUCCESS;
//     }
    
//     // Poll input state
//     InputEvent input = gInputHandler.poll();
//     if (input.quit) {
//         return SDL_APP_SUCCESS;
//     }
    
//     return SDL_APP_CONTINUE;
// }

// /* Helper: draw one frame of a 64x32 checkerboard, auto-scaled and centered. */
// static void DrawCheckerboard(SDL_Renderer* r)
// {
//     int outW = 0, outH = 0;
//     SDL_GetRenderOutputSize(r, &outW, &outH);

//     /* Integer scale to keep pixels crisp. Clamp to >= 1 so it always shows. */
//     const int scale = SDL_max(1, SDL_min(outW / CHIP_W, outH / CHIP_H));
//     const int viewW = scale * CHIP_W;
//     const int viewH = scale * CHIP_H;
//     const int offX  = (outW - viewW) / 2;
//     const int offY  = (outH - viewH) / 2;

//     /* Background (letterbox bars) */
//     SDL_SetRenderDrawColor(r, 20, 20, 20, 255);
//     SDL_RenderClear(r);

//     /* Checker colors */
//     SDL_Color light = { 240, 240, 240, 255 };
//     SDL_Color dark  = {  40,  40,  40, 255 };

//     /* Draw squares as scaled rectangles. */
//     SDL_FRect rect;
//     rect.w = (float)scale;
//     rect.h = (float)scale;

//     for (int y = 0; y < CHIP_H; ++y) {
//         for (int x = 0; x < CHIP_W; ++x) {
//             const int isLight = ((x + y) & 1) == 0;
//             const SDL_Color c = isLight ? light : dark;

//             SDL_SetRenderDrawColor(r, c.r, c.g, c.b, c.a);
//             rect.x = (float)(offX + x * scale);
//             rect.y = (float)(offY + y * scale);
//             SDL_RenderFillRect(r, &rect);
//         }
//     }

//     SDL_RenderPresent(r);
// }

// /* Runs once per frame. */
// SDL_AppResult SDL_AppIterate(void *appstate)
// {
//     (void)appstate;
//     DrawCheckerboard(gRenderer);
//     return SDL_APP_CONTINUE;
// }

// /* Runs once at shutdown. */
// void SDL_AppQuit(void *appstate, SDL_AppResult result)
// {
//     (void)appstate; (void)result;
//     /* SDL cleans up window/renderer automatically. */
// }
