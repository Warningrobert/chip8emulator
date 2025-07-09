//
// Created by robert on 7/9/25.
//

#ifndef CHIP_H
#define CHIP_H
#include <cstdint>
#include <stack>


class Chip {

public:
    Chip8();


private:
    uint8_t memory[4096];
    bool pixels[64][32];
    int pc;
    uint16_t I;
    std::stack<uint16_t> addressStack;
    uint8_t timer;
    uint8_t V[16];

};



#endif //CHIP_H
