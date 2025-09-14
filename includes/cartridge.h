#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include "bus.h"

class Bus;

class Cartridge{
private:
    Bus *bus;
    unsigned char *header;
    unsigned char *prg_rom;
    unsigned char *chr_rom;
    unsigned char *prg_banks;
    unsigned char *chr_banks;
    unsigned char mapper;
    bool ram;
public:
    ~Cartridge();
    void connectBus(Bus *bus);
    void read(const char *filename);
    unsigned char readMemory(unsigned short address);
    void writeMemory(unsigned short address, unsigned char value);
};




#endif
