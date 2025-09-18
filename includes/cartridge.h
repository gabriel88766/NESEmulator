#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include "bus.h"

class Bus;

class Cartridge{
private:
    Bus *bus;
    unsigned char *header;
    unsigned char *prg_rom;
    unsigned char *prg_ram;
    unsigned char *prg_banks;
    unsigned char *chr_banks;
    unsigned char *chr_rom;
    unsigned char mapper;
    void load(unsigned char *dst, unsigned char *src, int sz);
public:
    bool ram = false; //accessed by bus for openbus behavior
    ~Cartridge();
    void connectBus(Bus *bus);
    bool read(const char *filename);
    unsigned char readMemory(unsigned short address);
    void writeMemory(unsigned short address, unsigned char value);
};




#endif
