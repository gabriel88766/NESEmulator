#ifndef BUS_H
#define BUS_H
//NES BUS
#include "6502.h"

class CPU;

class Bus{
private:
    CPU *cpu;
    unsigned char memory[0xFFFF];// 0x800 is the right value, switch after Klaus test
public:
    Bus(){}
    void connectCPU(CPU *cpu);
    void readTest();
    unsigned char readAddress(unsigned short address);
    void writeAddress(unsigned short address, unsigned char value);
};

#endif
