#ifndef PPU_H
#define PPU_H

#include "bus.h"
#include "ImageMaker.h"
class Bus;
class Image;

class PPU{
private:
    Bus *bus;
    unsigned char regs[8]; 
    Image img;
    int frameCount = 0;
public:
    void connectBus(Bus *bus);
    unsigned char readMemory(unsigned short address);
    void writeMemory(unsigned short address, unsigned char value);
    void printFrame();
    void testMake();
};

#endif