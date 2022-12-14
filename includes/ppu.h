#ifndef PPU_H
#define PPU_H

#include "bus.h"
#include "ImageMaker.h"
class Bus;
class Image;

class PPU{
private:
    Bus *bus;
    unsigned char regs[10]; 
    unsigned char VRAM[0x800];
    Image *img;
    int frameCount = 0;
    bool is_read;
    unsigned char value;
    int write_ppu_status;
    void(PPU::*register_action[8])();
public:
    PPU();
    void connectBus(Bus *bus);
    unsigned char readMemory(unsigned short address);
    void writeMemory(unsigned short address, unsigned char value);
    void printFrame();
    void testMake();

    //regs functions
    void PPUCTRL(); //reg 0
    void PPUMASK(); //reg 1
    void PPUSTATUS(); //reg 2
    void OAMADDR(); // reg 3
    void OAMDATA(); // reg 4
    void PPUSCROLL(); //reg 5 6
    void PPUADDR(); //reg 7 8
    void PPUDATA(); //reg 9
};
// (obj.*function)();
#endif