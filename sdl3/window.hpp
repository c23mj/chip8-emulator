#pragma once
#include <cstdint>
#include <span>

using byte = uint8_t;

class Window {
public:
    // CHIP-8 grid size -- default 64 x 32
    Window(const char* title, int gridW, int gridH); 
    ~Window();

    // Draw a monochrome grid (on/off per cell). Returns false if user requested quit.
    bool draw(std::span<const byte> bits);

    // Check if window is still alive
    bool alive() const;

private:
    struct Impl;   // PIMPL to keep SDL out of headers
    Impl* impl_;
};
