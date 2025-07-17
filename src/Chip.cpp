//
// Created by robert on 7/9/25.
//

#include "Chip.h"
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <cstdlib>

Chip::Chip() : pc(512), I(0), timer(0) {

    srand(time(nullptr));

    for (int i = 0; i < 4096; i++) {
        memory[i] = 0;
    }

    for (int x = 0; x < 64; x++) {
        for (int y = 0; y < 32; y++) {
            pixels[x][y] = false;
        }
    }

    for (int i = 0; i < 16; i++) {
        V[i] = 0;
    }

    for (int i = 0; i < 80; i++) {
        memory[80 + i] = fontSet[i];
    }
    lastTimerUpdate = std::chrono::steady_clock::now();
}

bool Chip::loadROM(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }

    file.read(reinterpret_cast<char*>(&memory[512]), 4096 - 512);

    if (file.bad()) {
        std::cerr << "Error reading file " << filename << std::endl;
        return false;
    }

    file.close();
    return true;
}
void Chip::updateTimers() {
    updateOccured = false;
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTimerUpdate);

    if (elapsed.count() >= 16) {  // ~60Hz
        updateOccured = true;
        if (timer > 0) {
            timer--;
        }
        lastTimerUpdate = now;
    }
}

uint16_t Chip::fetchInstruction() {

    uint8_t firstByte = memory[pc];

    uint8_t secondByte = memory[pc + 1];

    uint16_t instruction = (firstByte << 8) | secondByte;

    pc += 2;

    return instruction;
}

void Chip::executeInstruction() {
    uint16_t instruction = fetchInstruction();
    decodeAndExecute(instruction);
}

void Chip::decodeAndExecute(uint16_t instruction) {
    std::cout << "PC: 0x" << std::hex << pc << " Instruction: 0x" << instruction << std::endl; // For debugging
    /*
     * CHIP-8 instructions are 16 bits (2 bytes) and follow this format:
     * Each instruction is made up of 4 nibbles (4-bit values):
     *
     * [FIRST][X][Y][N] or [FIRST][NNN] or [FIRST][X][NN]
     *
     * - FIRST nibble: Determines the instruction category (0x0 to 0xF)
     * - X: Second nibble, usually a register index (V0-VF)
     * - Y: Third nibble, usually a register index (V0-VF)
     * - N: Fourth nibble, a 4-bit immediate value
     * - NN: Last two nibbles combined, an 8-bit immediate value
     * - NNN: Last three nibbles combined, a 12-bit memory address
     */

    uint8_t op = (instruction >> 12) & 0xF;

    // Extract common operands
    uint8_t x = (instruction >> 8) & 0xF;
    uint8_t y = (instruction >> 4) & 0xF;
    uint8_t n = instruction & 0xF;
    uint8_t nn = instruction & 0xFF;
    uint16_t nnn = instruction & 0xFFF;

    switch (op) {
        case 0x0: {
            if (instruction == 0x00E0) {
                // Clear screen
                for (int x = 0; x < 64; ++x) {
                    for (int y = 0; y < 32; ++y) {
                        pixels[x][y] = false;
                    }
                }
            }
            // Return from subroutine
            else if (instruction == 0x00EE) {
                if (!addressStack.empty()) {
                    pc = addressStack.top();
                    addressStack.pop();
                }
            }
            break;
        }

        case 0x1: {
            // Jump to address
            pc = nnn;
            break;
        }

        // Call subroutine
        case 0x2: {
            addressStack.push(pc);  // Save current location
            pc = nnn;               // Jump to subroutine
            break;
        }

        // Skip if VX == NN
        case 0x3: {
            if (V[x] == nn) {
                pc += 2;  // Skip next instruction
            }
            break;
        }

        // Skip if VX != NN
        case 0x4: {
            if (V[x] != nn) {
                pc += 2;  // Skip next instruction
            }
            break;
        }
            // Skip if VX == VY
        case 0x5: {
            if (V[x] == V[y]) {
                pc += 2;  // Skip next instruction
            }
            break;
        }

        case 0x6: {
            // Set register VX to NN
            V[x] = nn;
            break;
        }

        case 0x7: {
            // Add NN to register VX
            V[x] += nn;
            break;
        }

        // Register operations
        case 0x8: {
            uint8_t operation = instruction & 0xF;
            switch (operation) {
                case 0x0: // Set VX = VY
                    V[x] = V[y];
                    break;
                case 0x4: // Add VY to VX (with carry)
                    {
                        uint16_t sum = V[x] + V[y];
                        V[0xF] = (sum > 255) ? 1 : 0;  // Set carry flag
                        V[x] = sum & 0xFF;             // Keep only lower 8 bits
                    }
                    break;
                case 0x5: // VX = VX - VY, set VF = NOT borrow
                    V[0xF] = (V[x] >= V[y]) ? 1 : 0;
                    V[x] -= V[y];
                    break;
                default:
                    std::cerr << "Unknown 8XY operation: 0x" << std::hex << operation << std::endl;
                    break;
            }
            break;
        }
            // Skip if VX != VY
        case 0x9: {
            if (V[x] != V[y]) {
                pc += 2;  // Skip next instruction
            }
            break;
        }

        case 0xA: {
            // Set index register I to NNN
            I = nnn;
            break;
        }
        case 0xC: {
            // Generate random number
            V[x] = (rand() % 256) & nn;
            break;
        }

        case 0xD: {
            // Draw N-byte sprite from memory location I at (VX, VY)
            uint8_t pos_x = V[x] % 64;
            uint8_t pos_y = V[y] % 32;

            V[0xF] = 0;

            for (uint8_t row = 0; row < n; ++row) {
                uint8_t sprite_byte = memory[I + row];

                for (uint8_t bit = 0; bit < 8; ++bit) {
                    uint8_t sprite_bit = (sprite_byte >> (7 - bit)) & 0x1;
                    if (sprite_bit == 1) {
                        uint8_t screen_x = (pos_x + bit) % 64;
                        uint8_t screen_y = (pos_y + row) % 32;

                        bool& pixel = pixels[screen_x][screen_y];
                        if (pixel) {
                            V[0xF] = 1;
                        }
                        pixel = !pixel;
                    }
                }
            }
            break;
        }

        // Timer operations
        case 0xF: {
            switch (nn) {
                case 0x55: // Store registers V0-VX in memory starting at I
                    for (uint8_t i = 0; i <= x; ++i) {
                        memory[I + i] = V[i];
                    }
                    break;
                case 0x65: // Load registers V0-VX from memory starting at I
                    for (uint8_t i = 0; i <= x; ++i) {
                        V[i] = memory[I + i];
                    }
                    break;
                case 0x07: // Set VX = delay timer
                    V[x] = timer;
                    break;
                case 0x15: // Set delay timer = VX
                    timer = V[x];
                    break;
                case 0x1E: // Add VX to index register I
                    I += V[x];
                    break;
                case 0x29: // Set I = location of sprite for digit VX
                    I = 80 + (V[x] & 0xF) * 5; // Font data starts at 0x50 (80), each char is 5 bytes
                    break;
                default:
                    std::cerr << "Unknown FX operation: 0x" << std::hex << nn << std::endl;
                    break;
            }
            break;
        }

        default:
            std::cerr << "Unknown instruction: 0x" << std::hex << instruction << std::endl;
            break;
    }
}

void Chip::renderDisplay() const {
    std::system("clear");

    for (int y = 0; y < 32; ++y) {
        for (int x = 0; x < 64; ++x) {
            std::cout << (pixels[x][y] ? '*' : ' ');
        }
        std::cout << std::endl;
    }

    std::cout << std::flush;
}