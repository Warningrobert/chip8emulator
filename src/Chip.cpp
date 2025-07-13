//
// Created by robert on 7/9/25.
//

#include "Chip.h"
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>

Chip::Chip() : pc(512), I(0), timer(0) {

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
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastTimerUpdate);

    if (elapsed.count() >= 16) {  // ~60Hz
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
    uint8_t op = (instruction >> 12) & 0xF;

    switch (op) {
        case 0x0: {
            if (instruction == 0x00E0) {
                for (int x = 0; x < 64; ++x) {
                    for (int y = 0; y < 32; ++y) {
                        pixels[x][y] = false;
                    }
                }
            }
            break;
        }

        case 0x1: {
            uint16_t addr = instruction & 0xFFF;
            pc = addr;
            break;
        }

        case 0x6: {
            uint8_t x = (instruction >> 8) & 0xF;
            uint8_t nn = instruction & 0xFF;
            V[x] = nn;
            break;
        }

        case 0x7: {
            uint8_t x = (instruction >> 8) & 0xF;
            uint8_t nn = instruction & 0xFF;
            V[x] += nn;
            break;
        }

        case 0xA: {
            uint16_t addr = instruction & 0xFFF;
            I = addr;
            break;
        }

        case 0xD: {
            uint8_t x_reg = (instruction >> 8) & 0xF;
            uint8_t y_reg = (instruction >> 4) & 0xF;
            uint8_t height = instruction & 0xF;

            uint8_t pos_x = V[x_reg] % 64;
            uint8_t pos_y = V[y_reg] % 32;

            V[0xF] = 0;

            for (uint8_t row = 0; row < height; ++row) {
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

        default:
            std::cerr << "Unknown instruction: 0x" << std::hex << instruction << std::endl;
            break;
    }
}