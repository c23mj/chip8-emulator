A (mostly) working implementation of an emulator (interpreter) for the
[CHIP-8 programming language](https://en.wikipedia.org/wiki/CHIP-8) in C++
using the [SDL3 graphics library](https://www.libsdl.org/).

Build and run:

```sh
cmake -S . -B build
cmake --build build
./chip8 spacejam.ch8
```

The CPU defaults to 6,000 instructions per second, matching the CHIP-8 Archive's
100 cycles per 60 Hz frame setting for `spacejam.ch8`. To choose a different
instruction rate, pass a positive integer as the second argument. For example,
`./chip8 spacejam.ch8 12000` runs at 12,000 instructions per second. Timers and
the display update at 60 Hz independently of the instruction rate.

<p align="center">
  <img
    src="https://github.com/user-attachments/assets/549f9280-c560-40ec-bb5a-0dcf1269fdd9"
    alt="CHIP-8 emulator screenshot"
    width="501"
    height="251"
  />
</p>
