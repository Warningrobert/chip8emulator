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