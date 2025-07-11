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