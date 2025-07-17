//
// Created by robert on 7/9/25.
//

#ifndef CHIP_H
#define CHIP_H
#include <chrono>
#include <cstdint>
#include <stack>
#include <string>


class Chip {

public:
    Chip();
    bool loadROM(const std::string& filename);
    void updateTimers();
    void executeInstruction();
    void renderDisplay() const;
    bool updateOccured = false;
    void setKeyPressed(uint8_t key, bool pressed);

private:
    uint8_t memory[4096];
    bool pixels[64][32];
    int pc;
    bool keyPressed[16];
    uint8_t soundTimer;
    uint16_t I;
    std::stack<uint16_t> addressStack;
    uint8_t timer;
    uint8_t V[16];
    std::chrono::steady_clock::time_point lastTimerUpdate;
    uint16_t fetchInstruction();
    void decodeAndExecute(uint16_t instruction);
    uint8_t fontSet[80] = {
        // Character '0'
        0xF0, // ****
        0x90, // *  *
        0x90, // *  *
        0x90, // *  *
        0xF0, // ****

        // Character '1'
        0x20, //   *
        0x60, //  **
        0x20, //   *
        0x20, //   *
        0x70, //  ***

        // Character '2'
        0xF0, // ****
        0x10, //    *
        0xF0, // ****
        0x80, // *
        0xF0, // ****

        // Character '3'
        0xF0, // ****
        0x10, //    *
        0xF0, // ****
        0x10, //    *
        0xF0, // ****

        // Character '4'
        0x90, // *  *
        0x90, // *  *
        0xF0, // ****
        0x10, //    *
        0x10, //    *

        // Character '5'
        0xF0, // ****
        0x80, // *
        0xF0, // ****
        0x10, //    *
        0xF0, // ****

        // Character '6'
        0xF0, // ****
        0x80, // *
        0xF0, // ****
        0x90, // *  *
        0xF0, // ****

        // Character '7'
        0xF0, // ****
        0x10, //    *
        0x20, //   *
        0x40, //  *
        0x40, //  *

        // Character '8'
        0xF0, // ****
        0x90, // *  *
        0xF0, // ****
        0x90, // *  *
        0xF0, // ****

        // Character '9'
        0xF0, // ****
        0x90, // *  *
        0xF0, // ****
        0x10, //    *
        0xF0, // ****

        // Character 'A'
        0xF0, // ****
        0x90, // *  *
        0xF0, // ****
        0x90, // *  *
        0x90, // *  *

        // Character 'B'
        0xE0, // ***
        0x90, // *  *
        0xE0, // ***
        0x90, // *  *
        0xE0, // ***

        // Character 'C'
        0xF0, // ****
        0x80, // *
        0x80, // *
        0x80, // *
        0xF0, // ****

        // Character 'D'
        0xE0, // ***
        0x90, // *  *
        0x90, // *  *
        0x90, // *  *
        0xE0, // ***

        // Character 'E'
        0xF0, // ****
        0x80, // *
        0xF0, // ****
        0x80, // *
        0xF0, // ****

        // Character 'F'
        0xF0, // ****
        0x80, // *
        0xF0, // ****
        0x80, // *
        0x80  // *
    };

};



#endif //CHIP_H
