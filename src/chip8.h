#ifndef CHIP8_H
#define CHIP8_H

#include <iostream>
#include <fstream>

class Chip8{
private:
    unsigned short opcode;
    unsigned char memory[4096];
    unsigned char V[16];
    unsigned short I;
    unsigned short pc;
    unsigned char delay_timer;
    unsigned char sound_timer;
    unsigned short stack[16];
    unsigned short sp;
    bool drawFlag;
    unsigned char key[16];
    uint8_t gfx[2048];

public:

    Chip8();
    ~Chip8();

    int LoadGame(std::string);
    void emulateCycle();
    uint8_t* getPixel();
    bool shouldDraw();
    void keyPressed(int, bool);
};

#endif