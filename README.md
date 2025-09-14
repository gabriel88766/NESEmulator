# NESEmulator
In development: NES Emulator - CPP - 

[1] Used to implement all instructions and addressing modes \
[2] Klaus functional test, used to test CPU 6502 \
[3] Undestanding the behavior of the overflow flag. \
[4] For APU, PPU, and some NES specific CPU behaviors \
[5] SDL2 Library, for multimedia \

references : \
[1] https://www.pagetable.com/c64ref/6502/ \
[2] https://github.com/Klaus2m5/6502_65C02_functional_tests \
[3] http://www.righto.com/2012/12/the-6502-overflow-flag-explained.html#:~:text=The%20definition%20of%20the%206502,%3E%20127%20or%20%3C%20%2D128. \
[4] https://www.nesdev.org/wiki \
[5] https://wiki.libsdl.org/SDL2/FrontPage \


# Prerequisites

- CMake >= 3.16
- Compiler:
  - Windows: MinGW (G++) or Visual Studio
  - Linux/macOS: GCC, Clang, or Xcode toolchain (macOS only)
- Git

# Downloading SDL (inside your project folder)

mkdir vendored
cd vendored
git clone https://github.com/libsdl-org/SDL
cd SDL
git checkout release-2.32.x

# Building the project

# Windows (with MinGW)
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build

# Linux / macOS
cmake -S . -B build
cmake --build build

# Applying changes (rebuilding after edits)
cmake --build build

# Output

The compiled binary is available at:

build/bin/emuNES

# Notes

- On Windows, if SDL was built as a shared library, copy SDL2.dll to build/bin so the binary runs.
- On Linux/macOS, the default generator is sufficient.
- After editing source files, just run `cmake --build build` to rebuild.