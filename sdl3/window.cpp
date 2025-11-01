#include "window.hpp"
#include <SDL3/SDL.h>
#include <stdexcept>
#include <string>
#include <algorithm>

struct Window::Impl {
    SDL_Window*   win   = nullptr;
    SDL_Renderer* ren   = nullptr;
    int           gridW = 0, gridH = 0; // logical grid (e.g., 64x32)
    bool          quit  = false;
};

static void sdlThrow(const char* msg) {
    throw std::runtime_error(std::string(msg) + ": " + SDL_GetError());
}

Window::Window(const char* title, int gridW, int gridH)
: impl_(new Impl) {
    impl_->gridW = gridW;
    impl_->gridH = gridH;

    if (!SDL_Init(SDL_INIT_VIDEO)) sdlThrow("SDL_Init failed");

    impl_->win = SDL_CreateWindow(title, 800, 600, SDL_WINDOW_RESIZABLE);
    if (!impl_->win) sdlThrow("SDL_CreateWindow failed");

    impl_->ren = SDL_CreateRenderer(impl_->win, nullptr);
    if (!impl_->ren) sdlThrow("SDL_CreateRenderer failed");

    SDL_SetAppMetadata("CHIP-8", "1.0", "com.example.chip8");
}

Window::~Window() {
    if (impl_) {
        if (impl_->ren) SDL_DestroyRenderer(impl_->ren);
        if (impl_->win) SDL_DestroyWindow(impl_->win);
        SDL_Quit();
        delete impl_;
    }
}

bool Window::alive() const { return impl_ && !impl_->quit; }

bool Window::draw(std::span<const byte> bits) {
    if (!impl_ || impl_->quit) return false;
    const int gridW = impl_->gridW, gridH = impl_->gridH;
    if ((int)bits.size() != gridW * gridH) return false;

    // 1) pump events so the window stays responsive
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT ||
            (e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_ESCAPE)) {
            impl_->quit = true;
            return false;
        }
    }

    // 2) compute integer scale + centering (crisp pixels)
    int outW = 0, outH = 0;
    SDL_GetRenderOutputSize(impl_->ren, &outW, &outH);
    const int scale = std::max(1, std::min(outW / gridW, outH / gridH));
    const int viewW = scale * gridW;
    const int viewH = scale * gridH;
    const int offX  = (outW - viewW) / 2;
    const int offY  = (outH - viewH) / 2;

    // 3) clear + draw
    SDL_SetRenderDrawColor(impl_->ren, 0, 0, 0, 255);
    SDL_RenderClear(impl_->ren);

    // black panel (letterbox)
    SDL_FRect panel { (float)offX, (float)offY, (float)viewW, (float)viewH };
    SDL_RenderFillRect(impl_->ren, &panel);

    // white pixels
    SDL_SetRenderDrawColor(impl_->ren, 255, 255, 255, 255);
    SDL_FRect px; px.w = (float)scale; px.h = (float)scale;
    for (int y = 0; y < gridH; ++y) {
        const int base = y * gridW;
        for (int x = 0; x < gridW; ++x) {
            if (bits[(size_t)(base + x)]) {
                px.x = (float)(offX + x * scale);
                px.y = (float)(offY + y * scale);
                SDL_RenderFillRect(impl_->ren, &px);
            }
        }
    }

    SDL_RenderPresent(impl_->ren);
    return true;
}
