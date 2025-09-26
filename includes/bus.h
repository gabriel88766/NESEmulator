#ifndef BUS_H
#define BUS_H
//NES BUS
#include "6502.h"
#include "cartridge.h"
#include "ppu.h"
#include "apu.h"

class CPU;
class Cartridge;
class PPU;
class APU;

class Bus{
private:
    int cb = 0;
    bool strobe;
    unsigned char regbut1;
    unsigned char regbut2;
public:
    CPU *cpu;
    Cartridge *cartridge;
    PPU *ppu;
    APU *apu;
    unsigned char open;
    unsigned char last_value;
    unsigned char button1;
    unsigned char button2;
    Bus(){}
    void connectCPU(CPU *cpu);
    void connectCartridge(Cartridge *cartridge);
    void connectPPU(PPU *ppu);
    void connectAPU(APU *apu);
    unsigned char readAddress(unsigned short address);
    void writeAddress(unsigned short address, unsigned char value);
    unsigned char readCartridge(unsigned short address);
    void writeCartridge(unsigned short address, unsigned char value);
};

#endif
