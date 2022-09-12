#ifndef BUS_H
#define BUS_H
//NES BUS
#include "6502.h"

class CPU;

class Bus{
private:
    CPU *cpu;
    unsigned char memory[0x800];
public:
    void connectCPU(CPU *cpu);

    unsigned char readAddress(unsigned short address);
    void writeAddress(unsigned short address, unsigned char value);
};

#endif
