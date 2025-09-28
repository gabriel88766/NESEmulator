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
    //for mapper 4 and ... ???
    
    bool chr = false;
    int nb = -1;
    bool b2k = false;
    bool irq4 = false;
    int rlirq4;
    int cnt4;
    bool reload = false;

    //mapper 1
    unsigned char ctrl1 = 0xC;
    unsigned char reg1 = 0; //lower 3 bits store the next bit, upper 5 bits store the value.
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
