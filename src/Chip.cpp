//
// Created by robert on 7/9/25.
//

#include "Chip.h"

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