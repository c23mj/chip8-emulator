#pragma once
#include <cstdint>
#include <span>

using byte = uint8_t;

struct DrawTiming {
    double commands_ms = 0;
    double present_ms = 0;
};

class Window {
public:
    // CHIP-8 grid size -- default 64 x 32
    Window(const char* title, int gridW, int gridH); 
    ~Window();

    // Draw a monochrome grid (on/off per cell). Returns false if user requested quit.
    bool draw(std::span<const byte> bits, DrawTiming* timing = nullptr);
    const char* rendererName() const;

    // Check if window is still alive
    bool alive() const;

private:
    struct Impl;   // PIMPL to keep SDL out of headers
    Impl* impl_;
};
