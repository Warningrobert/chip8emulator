# CHIP-8 Emulator

A CHIP-8 interpreter written in C++ that runs classic games and programs in your terminal.

## Features

**Complete instruction set** with all 35 standard CHIP-8 opcodes including:
- Arithmetic and logical operations
- Memory manipulation 
- Graphics rendering with XOR-based sprite drawing
- Subroutine calls with stack management
- Timer systems (delay and sound)
- Real-time keyboard input handling

**What it can run:**
- Pong (single player)
- Tic-Tac-Toe 
- IBM Logo test ROM
- Various homebrew games and demos

**Technical specifications:**
- 4KB memory emulation 
- 64x32 pixel display rendered as ASCII characters
- 16-key hexadecimal keypad mapping
- 60Hz timer system 
- Non-blocking keyboard input

## Building

```bash
mkdir build && cd build
cmake ..
make
```

## Usage

```bash
./chip8emulator <rom_file>
```

### Controls

The CHIP-8 keypad maps to your keyboard as follows:

```
CHIP-8 Keypad    →    Keyboard
1 2 3 C          →    1 2 3 4
4 5 6 D          →    Q W E R
7 8 9 E          →    A S D F
A 0 B F          →    Z X C V
```

**Game-specific controls:**
- **Pong**: '1' (up) and 'Q' (down) to control the paddle
- **Tic-Tac-Toe**: Keys 1-9 to place moves on the grid

## Implementation Details

Built following Tobias VL's excellent CHIP-8 guide, this emulator uses modern interpreter behavior for maximum ROM compatibility. The instruction decoder uses a switch-based approach with proper operand extraction, and the display system uses XOR-based pixel manipulation for authentic sprite rendering.

## Requirements

- C++11 compatible compiler
- Unix-like system (Linux, macOS) 
- CMake 3.31 or higher

## Resources

- [CHIP-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
- [Tobias VL's CHIP-8 Guide](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/)
- [CHIP-8 Archive](https://johnearnest.github.io/chip8Archive/) for ROMs

---

*Built as a learning exercise in emulator development and low-level programming.*
