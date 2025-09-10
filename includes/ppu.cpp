#include "ppu.h"
#include <cstdio>
#include <cstring>

void(PPU::*register_action[])() = {
    &PPU::PPUCTRL,
    &PPU::PPUMASK,
    &PPU::PPUSTATUS,
    &PPU::OAMADDR,
    &PPU::OAMDATA,
    &PPU::PPUSCROLL,
    &PPU::PPUADDR,
    &PPU::PPUDATA
};


Color color_pallete_1[] = {
    { 0x00, 0x00, 0x00},
    { 0xfc, 0xfc, 0xfc},
    { 0xf8, 0xf8, 0xf8},
    { 0xbc, 0xbc, 0xbc},
    { 0x7c, 0x7c, 0x7c},
    { 0xa4, 0xe4, 0xfc},
    { 0x3c, 0xbc, 0xfc},
    { 0x00, 0x78, 0xf8},
    { 0x00, 0x00, 0xfc},
    { 0xb8, 0xb8, 0xf8},
    { 0x68, 0x88, 0xfc},
    { 0x00, 0x58, 0xf8},
    { 0x00, 0x00, 0xbc},
    { 0xd8, 0xb8, 0xf8},
    { 0x98, 0x78, 0xf8},
    { 0x68, 0x44, 0xfc},
    { 0x44, 0x28, 0xbc},
    { 0xf8, 0xb8, 0xf8},
    { 0xf8, 0x78, 0xf8},
    { 0xd8, 0x00, 0xcc},
    { 0x94, 0x00, 0x84},
    { 0xf8, 0xa4, 0xc0},
    { 0xf8, 0x58, 0x98},
    { 0xe4, 0x00, 0x58},
    { 0xa8, 0x00, 0x20},
    { 0xf0, 0xd0, 0xb0},
    { 0xf8, 0x78, 0x58},
    { 0xf8, 0x38, 0x00},
    { 0xa8, 0x10, 0x00},
    { 0xfc, 0xe0, 0xa8},
    { 0xfc, 0xa0, 0x44},
    { 0xe4, 0x5c, 0x10},
    { 0x88, 0x14, 0x00},
    { 0xf8, 0xd8, 0x78},
    { 0xf8, 0xb8, 0x00},
    { 0xac, 0x7c, 0x00},
    { 0x50, 0x30, 0x00},
    { 0xd8, 0xf8, 0x78},
    { 0xb8, 0xf8, 0x18},
    { 0x00, 0xb8, 0x00},
    { 0x00, 0x78, 0x00},
    { 0xb8, 0xf8, 0xb8},
    { 0x58, 0xd8, 0x54},
    { 0x00, 0xa8, 0x00},
    { 0x00, 0x68, 0x00},
    { 0xb8, 0xf8, 0xd8},
    { 0x58, 0xf8, 0x98},
    { 0x00, 0xa8, 0x44},
    { 0x00, 0x58, 0x00},
    { 0x00, 0xfc, 0xfc},
    { 0x00, 0xe8, 0xd8},
    { 0x00, 0x88, 0x88},
    { 0x00, 0x40, 0x58},
    { 0xf8, 0xd8, 0xf8},
    { 0x78, 0x78, 0x78}
};

PPU::PPU(){
    img = new Image();  //for debugging
    img->makeImage(256,240); //also
    colors[0] = {0x00, 0x00, 0x00}; //also
    colors[1] = {0x3f, 0x3f, 0x3f};
    colors[2] = {0x7f, 0x7f, 0x7f};
    colors[3] = {0xcf, 0xcf, 0xcf}; //to here
    regs[2] = 0xA0;
}

void PPU::connectBus(Bus *bus){
    this->bus = bus;
}

unsigned char PPU::readMemory(unsigned short address){
    is_read = true;
    (this->*register_action[address & 0x7])();
    return retVal;
}

void PPU::writeMemory(unsigned short address, unsigned char value){
    is_read = false;
    this->value = value;
    (this->*register_action[address & 0x7])();
}

void PPU::printFrame(){
    if(!img){
        img = new Image();  //for debugging
        img->makeImage(256,240); //also
    }
    if(frameCount > 200) return; //don't want to make infinite frames, at least not now!
    char *filename = new char[30];
    memset(filename, 0, 30);
    sprintf(filename, "images/frame%d.bmp", frameCount++ );
    //make_frame here:
    for(int i=0;i<30;i++){
        for(int j=0;j<32;j++){
            writeTile(8*j, 8*i, VRAM[0x2000 + 32*i+j]);
        }
    }
    img->writeImage(filename);
    delete[] filename;
}

void PPU::writeTile(int x, int y, int spr){ //Write some tile in the image file, fetch color from 
    unsigned short memplc = 0x3C0 + 8*(x/32) + (y/32);
    unsigned char color = VRAM[0x2000 + memplc];
    if((x % 32) >= 16){
        if((y % 32) >= 16) color = (color >> 6) & 3;
        else color = (color >> 4) & 3;
    }else{
        if((y % 32) >= 16) color = (color >> 2) & 3;
        else color = color & 3;
    }
    colors[0] = color_pallete_1[VRAM[(0x3F00+color*4) & 0x3F1F]];
    colors[1] = color_pallete_1[VRAM[(0x3F00+color*4 + 1) & 0x3F1F]];
    colors[2] = color_pallete_1[VRAM[(0x3F00+color*4 + 2) & 0x3F1F]];
    colors[3] = color_pallete_1[VRAM[(0x3F00+color*4 + 3) & 0x3F1F]];


    int offset = 0x6000+16*spr;
    if(regs[0] & 0x10) offset += 0x1000;
    for(int j=0;j<8;j++){
        unsigned char bytesl = bus->readAddress(offset+j); 
        unsigned char bytesr = bus->readAddress(offset+j+8);
        for(int k=0; k < 8; k++){
            unsigned curColor = (bytesl & (1 << (7-k)))? 1 : 0;
            curColor += (bytesr & (1 << (7-k)))? 2 : 0;
            img->setPixel(x+k,240 - (y+j), colors[curColor]);
        }
    }
}

void PPU::writeOAM(unsigned short address, unsigned char value){
    OAM[address] = value;
}

void PPU::vblank(){
    printFrame();
    if(regs[0] & 0x80){
        bus->setNMI();
    }
    regs[2] |= 0x80;
}

//regs functions
void PPU::PPUCTRL(){
    if(bus->getCycles() >= 30000) regs[0] = value;
}
void PPU::PPUMASK(){
    if(bus->getCycles() >= 30000) regs[1] = value;
}
void PPU::PPUSTATUS(){
    retVal = regs[2];
    regs[2] &= 0x7F;
    write_ppu_status = 0;
}
void PPU::OAMADDR(){
    if(bus->getCycles() >= 30000) regs[3] = value;
}
void PPU::OAMDATA(){
    if(is_read){
        retVal = regs[4];
    }else{
        if(bus->getCycles() >= 30000) regs[4] = value;
    }
}
void PPU::PPUSCROLL(){
    if(write_ppu_status == 0) regs[5] = value;
    else regs[6] = value;
    write_ppu_status ^= 1;
}
void PPU::PPUADDR(){//value *((unsigned short *)(regs+7));
    if(write_ppu_status == 0) regs[7] = value; //high byte
    else regs[8] = value; //low byte
    write_ppu_status ^= 1;
}
void PPU::PPUDATA(){
    int inc = regs[0] & 0x4 ? 32 : 1;// : 32;
    unsigned short address = regs[7];
    address <<= 8;
    address |= regs[8];
    address &= 0x3FFF;
    if(is_read){
        if(address < 0x2000){
            //not used yet.
        }else if(address < 0x3000){
            retVal = VRAM[address];
        }else if(address < 0x3F00){
            retVal = VRAM[address & 0x2FFF]; //mirror
        }else{
            retVal = VRAM[address];
        }
    }else{
        if(address < 0x2000){
            //not used yet.
        }else if(address < 0x3000){
            VRAM[address] = value;
        }else if(address < 0x3F00){
            VRAM[address & 0x2FFF] = value;
        }else{
            VRAM[address] = value;
        }
    }


    unsigned char nv = regs[8] + inc;
    if(nv < regs[8]) regs[7]++;
    regs[8] = nv; 
}