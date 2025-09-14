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
    unsigned char tile[8][8];
    unsigned char regs[10]; 
    unsigned char VRAM[0x4000]; //not accurate, however it will ease the program
    unsigned char OAM[0x100];
    unsigned short nametables[512][480]; //update after writting on VRAM
    bool opaque[512][480];
    bool isopaque[256][240];
    int frameCount = 0;
    bool is_read;
    unsigned char value;
    unsigned char retVal;
    unsigned char buffer;
    int write_ppu_status;
public:
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
    void setVblank();
    void clearVblank();
    //regs functions
    void PPUCTRL(); //reg 0
    void PPUMASK(); //reg 1
    void PPUSTATUS(); //reg 2
    void OAMADDR(); // reg 3
    void OAMDATA(); // reg 4
    void PPUSCROLL(); //reg 5 6
    void PPUADDR(); //reg 7 8
    void PPUDATA(); //reg 9
    void move();
};
// (obj.*function)();
#endif