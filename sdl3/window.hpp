#pragma once
#include <cstdint>
#include <span>

using byte = uint8_t; // or keep your alias consistent with the rest of your code

class Window {
public:
    Window(const char* title, int gridW, int gridH); // CHIP-8 grid size, e.g. 64x32
    ~Window();

    // Draw a monochrome grid (0/1 per cell). Returns false if user requested quit.
    bool draw(std::span<const byte> bits);

    // Optional: check if still alive without drawing.
    bool alive() const;

private:
    struct Impl;   // PIMPL to keep SDL out of headers
    Impl* impl_;
};
