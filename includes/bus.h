#ifndef BUS_H
#define BUS_H
//NES BUS
#include "6502.h"
#include "cartridge.h"
#include "ppu.h"

class CPU;
class Cartridge;
class PPU;

class Bus{
private:
    CPU *cpu;
    Cartridge *cartridge;
    PPU *ppu;
    unsigned char memory[0x800];
public:
    Bus(){}
    void connectCPU(CPU *cpu);
    void connectCartridge(Cartridge *cartridge);
    void connectPPU(PPU *ppu);
    unsigned char readAddress(unsigned short address);
    void writeAddress(unsigned short address, unsigned char value);
};

#endif
