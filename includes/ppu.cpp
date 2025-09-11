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
    {0x7C, 0x7C, 0x7C},
    {0x00, 0x00, 0xFC},
    {0x00, 0x00, 0xBC},
    {0x44, 0x28, 0xBC},
    {0x94, 0x00, 0x84},
    {0xA8, 0x00, 0x20},
    {0xA8, 0x10, 0x00},
    {0x88, 0x14, 0x00},
    {0x50, 0x30, 0x00},
    {0x00, 0x78, 0x00},
    {0x00, 0x68, 0x00},
    {0x00, 0x58, 0x00},
    {0x00, 0x40, 0x58},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},

    {0xBC, 0xBC, 0xBC},
    {0x00, 0x78, 0xF8},
    {0x00, 0x58, 0xF8},
    {0x68, 0x44, 0xFC},
    {0xD8, 0x00, 0xCC},
    {0xE4, 0x00, 0x58},
    {0xF8, 0x38, 0x00},
    {0xE4, 0x5C, 0x10},
    {0xAC, 0x7C, 0x00},
    {0x00, 0xB8, 0x00},
    {0x00, 0xA8, 0x00},
    {0x00, 0xA8, 0x44},
    {0x00, 0x88, 0x88},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},

    {0xF8, 0xF8, 0xF8},
    {0x3C, 0xBC, 0xFC},
    {0x68, 0x88, 0xFC},
    {0x98, 0x78, 0xF8},
    {0xF8, 0x78, 0xF8},
    {0xF8, 0x58, 0x98},
    {0xF8, 0x78, 0x58},
    {0xFC, 0xA0, 0x44},
    {0xF8, 0xB8, 0x00},
    {0xB8, 0xF8, 0x18},
    {0x58, 0xD8, 0x54},
    {0x58, 0xF8, 0x98},
    {0x00, 0xE8, 0xD8},
    {0x78, 0x78, 0x78},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00},

    {0xFC, 0xFC, 0xFC},
    {0xA4, 0xE4, 0xFC},
    {0xB8, 0xB8, 0xF8},
    {0xD8, 0xB8, 0xF8},
    {0xF8, 0xB8, 0xF8},
    {0xF8, 0xA4, 0xC0},
    {0xF0, 0xD0, 0xB0},
    {0xFC, 0xE0, 0xA8},
    {0xF8, 0xD8, 0x78},
    {0xD8, 0xF8, 0x78},
    {0xB8, 0xF8, 0xB8},
    {0xB8, 0xF8, 0xD8},
    {0x00, 0xFC, 0xFC},
    {0xF8, 0xD8, 0xF8},
    {0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00}
};

PPU::PPU(){
    memset(VRAM, 0, sizeof(VRAM));
    memset(OAM, 0, sizeof(OAM));
    img = new Image();  
    img->makeImage(256,240);
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
            writeTile(8*j, 8*i);
        }
    }
    //TODO write sprites!
    img->writeImage(filename);
    delete[] filename;
}

void PPU::writeTile(int x, int y){ //Write some tile in the image file, fetch color from 
    int rx = x + regs[5];
    int ry = y + regs[6];
    unsigned short nametable = 0x2000;// + (regs[0] & 3) * 0x400;
    if(rx >= 256) nametable += 0x400, rx -= 256;
    if(ry >= 240) nametable += 0x800, ry -= 240;
    unsigned short memplc1 = nametable + 32 * (ry/8) + rx/8;
    unsigned short memplc2 = nametable + 0x3C0 + (rx/32) + 8*(ry/32);
    unsigned char color_pat = VRAM[memplc2];
    if((ry % 32) >= 16){
        if((rx % 32) >= 16) color_pat = (color_pat >> 6) & 3;
        else color_pat = (color_pat >> 4) & 3;
    }else{
        if((rx % 32) >= 16) color_pat = (color_pat >> 2) & 3;
        else color_pat = color_pat & 3;
    }
    colors[0] = color_pallete_1[VRAM[0x3F00]];
    // colors[0] = color_pallete_1[VRAM[(0x3F00+color_pat*4) & 0x3F1F]];
    colors[1] = color_pallete_1[VRAM[(0x3F00+color_pat*4 + 1) & 0x3F1F]];
    colors[2] = color_pallete_1[VRAM[(0x3F00+color_pat*4 + 2) & 0x3F1F]];
    colors[3] = color_pallete_1[VRAM[(0x3F00+color_pat*4 + 3) & 0x3F1F]];


    int offset = 0x6000+16*VRAM[memplc1];
    if(regs[0] & 0x10) offset += 0x1000;
    for(int j=0;j<8;j++){
        unsigned char bytesl = bus->readAddress(offset+j); 
        unsigned char bytesr = bus->readAddress(offset+j+8);
        for(int k=0; k < 8; k++){
            unsigned curColor = (bytesl & (1 << (7-k)))? 1 : 0;
            curColor += (bytesr & (1 << (7-k)))? 2 : 0;
            color[x+k][y+j] = curColor;
            img->setPixel(x+k, 240 - (y+j), colors[curColor]);
        }
    }
}

void PPU::writeSprites(){

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
    if(bus->getCycles() >= 29658) regs[0] = value;
}
void PPU::PPUMASK(){
    if(bus->getCycles() >= 29658) regs[1] = value;
}
void PPU::PPUSTATUS(){
    retVal = regs[2];
    regs[2] &= 0x7F;
    write_ppu_status = 0;
}
void PPU::OAMADDR(){
    regs[3] = value;
}
void PPU::OAMDATA(){
    if(is_read){
        retVal = regs[4];
    }else{
        regs[4] = value;
    }
}
void PPU::PPUSCROLL(){
    if(bus->getCycles() >= 29658){
        if(write_ppu_status == 0) regs[5] = value; //xx
        else regs[6] = value; //yy
        write_ppu_status ^= 1;
    }    
}
void PPU::PPUADDR(){//value *((unsigned short *)(regs+7));
    if(bus->getCycles() >= 29658){
        if(write_ppu_status == 0) regs[7] = value; //high byte
        else regs[8] = value; //low byte
        write_ppu_status ^= 1;
    }
}
void PPU::PPUDATA(){
    int inc = regs[0] & 0x4 ? 32 : 1;// : 32;
    unsigned short address = regs[7];
    address <<= 8;
    address |= regs[8];
    address &= 0x3FFF;
    if(is_read){
        retVal = buffer;
        if(address < 0x2000){
            //not used yet.
        }else if(address < 0x3000){
            buffer = VRAM[address];
        }else if(address < 0x3F00){
            buffer = VRAM[address & 0x2FFF]; //mirror
        }else{
            retVal = buffer = VRAM[address & 0x3F1F];
        }
    }else{
        if(address < 0x2000){
            //not used yet.
        }else if(address < 0x3000){
            VRAM[address] = value;
        }else if(address < 0x3F00){
            VRAM[address & 0x2FFF] = value;
        }else{
            VRAM[address & 0x3F1F] = value;
        }
    }


    unsigned char nv = regs[8] + inc;
    if(nv < regs[8]) regs[7]++;
    regs[8] = nv; 
}