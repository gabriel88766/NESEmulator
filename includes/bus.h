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
    CPU *cpu;
    Cartridge *cartridge;
    PPU *ppu;
    APU *apu;
    unsigned char memory[0x800];
    int cb = 0;
    bool strobe;
    unsigned char regbut;
public:
    unsigned char buttons;
    Bus(){}
    void connectCPU(CPU *cpu);
    void connectCartridge(Cartridge *cartridge);
    void connectPPU(PPU *ppu);
    void connectAPU(APU *apu);
    unsigned char readAddress(unsigned short address);
    void writeAddress(unsigned short address, unsigned char value);
    void setNMI();
    long long int getCycles();
    void dumpStack();
};

#endif
