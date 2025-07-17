#include <iostream>
#include <thread>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <map>

#include "src/Chip.h"

uint8_t mapKeyToChip8(char key) {
    switch(key) {
        case '1': return 0x1;
        case '2': return 0x2;
        case '3': return 0x3;
        case '4': return 0xC;
        case 'q': case 'Q': return 0x4;
        case 'w': case 'W': return 0x5;
        case 'e': case 'E': return 0x6;
        case 'r': case 'R': return 0xD;
        case 'a': case 'A': return 0x7;
        case 's': case 'S': return 0x8;
        case 'd': case 'D': return 0x9;
        case 'f': case 'F': return 0xE;
        case 'z': case 'Z': return 0xA;
        case 'x': case 'X': return 0x0;
        case 'c': case 'C': return 0xB;
        case 'v': case 'V': return 0xF;
        default: return 0xFF;
    }
}

char checkKeyPress() {
    struct termios oldt, newt;
    char ch = 0;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    int oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    return ch;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <rom_file>" << std::endl;
        return 1;
    }

    Chip chip;

    if (!chip.loadROM(argv[1])) {
        return 1;
    }

    std::cout << "ROM loaded successfully!" << std::endl;
    std::cout << "Controls: 1234/QWER/ASDF/ZXCV (press keys for input)" << std::endl;

    std::map<uint8_t, int> keyFrameCount;

    while (true) {
        // Decrease frame count for all keys
        for (int i = 0; i < 16; i++) {
            if (keyFrameCount[i] > 0) {
                keyFrameCount[i]--;
                chip.setKeyPressed(i, true);
            } else {
                chip.setKeyPressed(i, false);
            }
        }

        // Check for key input
        char key = checkKeyPress();
        if (key != -1 && key != EOF) {
            uint8_t chip8Key = mapKeyToChip8(key);
            if (chip8Key != 0xFF) {
                keyFrameCount[chip8Key] = 10; // Key stays pressed for 10 frames
            }
        }

        chip.updateTimers();
        if (chip.updateOccured) {
            chip.renderDisplay();
        }

        chip.executeInstruction();
        std::this_thread::sleep_for(std::chrono::microseconds(500));
    }

    return 0;
}