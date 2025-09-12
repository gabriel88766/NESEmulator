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
    //make_frame here:
    if(regs[1] & 0x08){
        for(int i=0;i<30;i++){
            for(int j=0;j<32;j++){
                // if(j == 0 && (!(regs[1] & 0x2))) continue; //implement later, PPUMASK bit 1
                writeTile(8*j, 8*i);
            }
        }
    }
    if(regs[1] & 0x10) writeSprites();
}

void PPU::writeTile(int x, int y){ //Write some tile in the image file, fetch color from 
    int rx = x + regs[5] + (regs[0] & 1 ? 256 : 0);
    int ry = y + regs[6] + (regs[0] & 1 ? 240 : 0);
    unsigned short nametable = 0x2000; //+ (regs[0] & 3) * 0x400;
    if(rx >= 256) rx -= 256;
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
            isopaque[x+k][y+j] = curColor > 0; 
            framebuffer[x+k][y+j] = colors[curColor];
        }
    }
}

void PPU::writeSprites(){
    bool mode = regs[0] & 0x20 ? true : false;//true means 8x16, false means 8x8
    unsigned short table = (regs[0] & 8)  && (!mode) ? 0x1000 : 0; 
    for(int i=63;i>=0;i--){
        unsigned char y = OAM[4*i];
        unsigned char x = OAM[4*i + 3];
        bool hidden = OAM[4*i + 2] & 0x20 ? true : false;
        bool flipx = OAM[4*i + 2] & 0x40 ? true : false;
        bool flipy = OAM[4*i + 2] & 0x80 ? true : false;
        int color_pat = OAM[4*i + 2] & 3;
        // colors[0] = color_pallete_1[VRAM[0x3F00]]; //doesn't matter
        colors[1] = color_pallete_1[VRAM[(0x3F10+color_pat*4 + 1) & 0x3F1F]];
        colors[2] = color_pallete_1[VRAM[(0x3F10+color_pat*4 + 2) & 0x3F1F]];
        colors[3] = color_pallete_1[VRAM[(0x3F10+color_pat*4 + 3) & 0x3F1F]];
        unsigned short offset = 0x6000 + table; 

        int ysz;
        if(mode){
            ysz = 16;
            if(OAM[4*i + 1] & 1) offset += 0x1000;
            offset += 16 * (OAM[4*i+1] & 0xFE);
        }else{
            ysz = 8;
            offset += 16 * OAM[4*i+1]; 
        }
        for(int j=0;j<ysz;j++){
            unsigned char bytesl = bus->readAddress(offset+j); 
            unsigned char bytesr = bus->readAddress(offset+j+8);
            for(int k=0;k<8;k++){
                unsigned curColor = (bytesl & (1 << (7-k)))? 1 : 0;
                curColor += (bytesr & (1 << (7-k)))? 2 : 0;
                // curColor = 2;
                int rx = flipx ? x + 8 - k : x + k;
                int ry = flipy ? y + ysz - j : y + j;
                if(rx >= 256 || ry >= 240 || curColor == 0) continue;
                if(hidden){
                    if(!isopaque[rx][ry]) framebuffer[rx][ry] = colors[curColor];
                }else{
                    framebuffer[rx][ry] = colors[curColor];
                }
                if(i == 0 && curColor != 0 && isopaque[rx][ry]){
                     regs[2] |= 0x40;
                }
            }
        }
    
    }
}

void PPU::writeOAM(unsigned char value){
    OAM[regs[3]++] = value;
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
    if(is_read);
    else regs[3] = value;
}
void PPU::OAMDATA(){
    if(is_read){
        retVal = OAM[regs[3]];
    }else{
        writeOAM(value);
        // regs[4] = value;
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
            if(!(address & 3)) retVal = buffer = VRAM[address & 0x3F0F];
            else retVal = buffer = VRAM[address & 0x3F1F];
        }
    }else{
        if(address < 0x2000){
            //not used yet.
        }else if(address < 0x3000){
            VRAM[address] = value;
        }else if(address < 0x3F00){
            VRAM[address & 0x2FFF] = value;
        }else{
            if(!(address & 3)) VRAM[address & 0x3F0F] = value;
            else VRAM[address & 0x3F1F] = value;
        }
    }


    unsigned char nv = regs[8] + inc;
    if(nv < regs[8]) regs[7]++;
    regs[8] = nv; 
}