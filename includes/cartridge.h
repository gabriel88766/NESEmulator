#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include "bus.h"

class Bus;

class Cartridge{
private:
    Bus *bus;
    unsigned char *header;
    unsigned char **prg_rom;
    unsigned char **chr_rom;
    unsigned char *prg_ram;
    unsigned char *prg_banks;
    unsigned char *chr_banks;
    unsigned char *chr_ram;
    unsigned char mapper;
    void load(unsigned char *dst, unsigned char *src, int sz);
    //for mapper 4 and ... ???
    
    bool chr = false;
    int nb = -1;
    bool b2k = false;
    bool irq4 = false;
    int rlirq4;
    int cnt4;
    bool reload = false;
public:
    bool I = false;
    bool ram = false; //accessed by bus for openbus behavior
    ~Cartridge();
    void connectBus(Bus *bus);
    bool read(const char *filename);
    unsigned char readMemory(unsigned short address);
    void writeMemory(unsigned short address, unsigned char value);
    void Clockmm3();
};




#endif
