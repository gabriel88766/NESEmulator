#ifndef PPU_H
#define PPU_H

#include "bus.h"
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
    Color colors[4];
    unsigned char regs[10]; 
    unsigned char VRAM[0x4000]; //not accurate, however it will ease the program
    unsigned char OAM[0x100];
    unsigned short nametables[512][480]; //update after writting on VRAM
    bool opaque[512][480];
    bool isopaque[256][240];
    int frameCount = 0;
    bool is_read = false;
    unsigned char value = 0;
    unsigned char retVal = 0;
    unsigned char buffer = 0;
    int write_ppu_status = 0;
    int xx = 0, yy = 0;
    int colx = -1, coly = -1;
public:
    bool okVblank = false;
    bool horizontal; //set by cartridge
    Color framebuffer[256][240];
    PPU();
    void changeNametables(unsigned short address, unsigned char value);
    void connectBus(Bus *bus);
    unsigned char readMemory(unsigned short address);
    void writeMemory(unsigned short address, unsigned char value);
    void printFrame();
    void writeTile(int x, int y);
    void writeTiles();
    void writeSprites();
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
};
// (obj.*function)();
#endif