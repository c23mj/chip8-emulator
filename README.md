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
100 cycles per 60 Hz frame setting for `spacejam.ch8`. The archive specifies
1,000 cycles per frame for `1dcell.ch8`, so run that ROM with
`./chip8 1dcell.ch8 60000`. Pass an instruction rate as the second argument
for other ROMs. Timers and the display update at 60 Hz independently of the
instruction rate.

To measure speed, run `./chip8 spacejam.ch8 --profile`. Once per second it prints
the actual instruction and frame rates, time spent drawing pixels, and time spent
presenting the frame.

<p align="center">
  <img
    src="https://github.com/user-attachments/assets/549f9280-c560-40ec-bb5a-0dcf1269fdd9"
    alt="CHIP-8 emulator screenshot"
    width="501"
    height="251"
  />
</p>
