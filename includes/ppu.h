#ifndef PPU_H
#define PPU_H

#include "bus.h"
#include <vector>
class Bus;



struct Color{
    unsigned char R;
    unsigned char G;
    unsigned char B;
    //Alpha default FF
};

class PPU{
private:
    Bus *bus;
    
    unsigned char OAM[0x100];
    bool sprzr[256];
    unsigned short mp[512][480];
    unsigned char mp2[512][480];
    std::vector<std::pair<unsigned short, bool>> spr[256];
    bool is_read = false;
    unsigned char value = 0;
    unsigned char retVal = 0;
    unsigned char buffer = 0;
    unsigned char openbus = 0;
    long long int bus_set = -1;
    int bst;
    int wreg = 0;
    unsigned short vreg = 0;
    int xx = 0, yy = 0;
    int sx = 0, sy = 0;
    unsigned short treg;
    unsigned short xreg;
    unsigned long long int spset = 0; //unused
    char even = 1;
    bool hit = false;
    
    

public:
unsigned char regs[10]; 
    const int HORIZONTAL = 0, VERTICAL = 1, FOUR_SCREEN = 2, SINGLE_SCREEN = 3;
    unsigned char VRAM[0x1000]; //exposed, so cartridge can write via bus
    unsigned char pmem[0x20];
    bool okVblank = false;
    int mirror; //set by cartridge. magic number : 0 = horizontal, 1 = vertical, 2 = four pages, 3 = single page.
    Color framebuffer[256][240];
    PPU();
    
    void connectBus(Bus *bus);
    unsigned char readMemory(unsigned short address);
    void writeMemory(unsigned short address, unsigned char value);
    void writeOAM(unsigned char value);
    
    //regs functions
    void PPUCTRL(); //reg 0
    void PPUMASK(); //reg 1
    void PPUSTATUS(); //reg 2
    void OAMADDR(); // reg 3
    void OAMDATA(); // reg 4
    void PPUSCROLL(); //reg 5 6
    void PPUADDR(); //reg 7 8
    void PPUDATA(); //reg 9

    //timing
    void setVblank();
    void clearVblank();
    void move();

    //print frame
    void evaluateScroll();
    void evaluateScrollY();
    void evaluateScrollX();
    void evaluateSprites(int yy);
    //misc
    void powerON();
    unsigned short getAddress(unsigned short addr);
};
#endif