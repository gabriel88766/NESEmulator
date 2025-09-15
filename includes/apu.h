#ifndef APU_H
#define APU_H

#include "bus.h"

class Bus;

class APU{
private:
    unsigned char reg[0x20];
    Bus *bus;
    int tc;
    bool F, I;
    bool en[5];
    double phase[3];
    unsigned char len[5];
    unsigned char len2;
    unsigned char len_table[32] = {10, 254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14, 12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30};
public:
    APU();
    void reset();
    void connectBus(Bus *bus);
    unsigned char readMemory(unsigned short address);
    void writeMemory(unsigned short address, unsigned char value);
    void tick();
    void linearCounter();
    void sweep();
    void lenCounter();
    void getSampling(short *buffer, int length, double rate);
};



#endif