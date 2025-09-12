#ifndef APU_H
#define APU_H

#include "bus.h"

class Bus;

class APU{
private:
    unsigned char reg[0x20];
    Bus *bus;
public:
    APU();
    void connectBus(Bus *bus);
    unsigned char readMemory(unsigned short address);
    void writeMemory(unsigned short address, unsigned char value);
};



#endif