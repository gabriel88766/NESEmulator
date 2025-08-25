#ifndef PPU_H
#define PPU_H

#include "bus.h"
#include "ImageMaker.h"
class Bus;
class Image;



class PPU{
private:
    Bus *bus;
    Color colors[4];
    unsigned char sprite[8][8];
    unsigned char regs[10]; 
    unsigned char VRAM[0x800];
    Image *img;
    int frameCount = 0;
    bool is_read;
    unsigned char value;
    unsigned char retVal;
    int write_ppu_status;
    
public:
    PPU();
    void connectBus(Bus *bus);
    unsigned char readMemory(unsigned short address);
    void writeMemory(unsigned short address, unsigned char value);
    void printFrame();
    void writeSprite(int x, int y, int spr);
    void testMake();
    void writeSprite(int spr);
    void vblank();
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