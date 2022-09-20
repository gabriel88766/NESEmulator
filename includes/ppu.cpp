#include "ppu.h"
#include <cstdio>
#include <cstring>

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

unsigned char bit_mask[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

PPU::PPU(){
    img = new Image();
    img->makeImage(256,240);
    register_action[0] = &PPUCTRL;
    register_action[1] = &PPUMASK;
    register_action[2] = &PPUSTATUS;
    register_action[3] = &OAMADDR;
    register_action[4] = &OAMDATA;
    register_action[5] = &PPUSCROLL;
    register_action[6] = &PPUADDR;
    register_action[7] = &PPUDATA;
    regs[2] = 0xA0;
}

void PPU::connectBus(Bus *bus){
    this->bus = bus;
}

unsigned char PPU::readMemory(unsigned short address){
    is_read = true;
    unsigned char returned_value = regs[address & 0x07];
    (this->*register_action[address & 0x7])();
    return returned_value;
}

void PPU::writeMemory(unsigned short address, unsigned char value){
    is_read = false;
    this->value = value;
    (this->*register_action[address & 0x7])();
}

void PPU::printFrame(){
    if(frameCount > 200) return;
    char *filename = new char[30];
    memset(filename, 0, 30);
    sprintf(filename, "images/frame%d.bmp", frameCount++ );
    printf("%s", filename);
    //make_frame here:
    




    img->writeImage(filename);
    delete[] filename;
}

void PPU::testMake(){ //display all sprites in grayscale;
    Color cols[] = {{0x00,0x00,0x00}, {0x3f,0x3f,0x3f}, {0x7f,0x7f,0x7f}, {0xcf,0xcf,0xcf}};
    img->makeImage(256, 128);
    for(int i = 0; i< 0x2000; i += 16){
        int y = (i/0x200)*8;
        int x = (i % 0x200)/2;
        unsigned char bytesl[8], bytesr[8], result[8][8];
        for(int j=0;j<8;j++){
            bytesl[j] = bus->readAddress(0x6000+i+j); 
            bytesr[j] = bus->readAddress(0x6000+i+j+8);
            for(int k=1; k <= 8; k++){
                result[j][k-1] = (bytesl[j] & bit_mask[8-k])? 1 : 0;
                result[j][k-1] += (bytesr[j] & bit_mask[8-k])? 2 : 0;
                img->setPixel(x+k-1, y+7-j, cols[result[j][k-1]]);
            }
        }
    }
    img->writeImage("images/test2.bmp"); 
}


//regs functions
void PPU::PPUCTRL(){
    regs[0] = value;
}
void PPU::PPUMASK(){
    
}
void PPU::PPUSTATUS(){
    regs[2] &= 0x7F;
    write_ppu_status = 0;
}
void PPU::OAMADDR(){
    
}
void PPU::OAMDATA(){
    
}
void PPU::PPUSCROLL(){
    if(write_ppu_status == 0) regs[5] = value;
    else regs[6] = value;
    write_ppu_status ^= 1;
}
void PPU::PPUADDR(){//value *((unsigned short *)(regs+7));
    if(write_ppu_status == 0) regs[7] = value;
    else regs[8] = value;
    write_ppu_status ^= 1;
}
void PPU::PPUDATA(){
    printf("Hello wordl");
}