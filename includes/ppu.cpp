#include "ppu.h"
#include <cstdio>
#include <cstring>
#include <cassert>

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
    memset(isopaque, 0, sizeof(isopaque));
    memset(opaque, 0, sizeof(opaque));
    memset(nametables, 0, sizeof(nametables));
    memset(regs, 0, sizeof(regs));
    memset(framebuffer, 0, sizeof(framebuffer));
    // regs[2] = 0xA0;
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
    openbus = value;
}

void PPU::writeOAM(unsigned char value){
    OAM[regs[3]++] = value;
}


//regs functions
void PPU::PPUCTRL(){
    if(is_read) retVal = openbus;
    else if(bus->getCycles() >= 29658){
        if(((regs[0] & 0x80) == 0) && (value & 0x80) && (regs[2] & 0x80)) bus->setNMI();
        regs[0] = value;
        treg &= 0x73FF;
        treg |= ((value & 3) << 10) & 0xC00;
    }
}   
void PPU::PPUMASK(){
    if(is_read) retVal = openbus;
    else if(bus->getCycles() >= 29658) regs[1] = value;
}
void PPU::PPUSTATUS(){
    openbus = retVal = regs[2] | (0x1F & openbus);
    bus_set = bus->getCycles();
    regs[2] &= 0x7F;
    wreg = 0;
}
void PPU::OAMADDR(){
    if(is_read) retVal = openbus;
    else regs[3] = value;
}
void PPU::OAMDATA(){
    if(is_read){
        openbus = retVal = OAM[regs[3]];
        bus_set = bus->getCycles();
    }else{
        writeOAM(value);
    }
}
void PPU::PPUSCROLL(){
    if(is_read) retVal = openbus;
    else{
        if(bus->getCycles() >= 29658){
            if(wreg == 0){
                treg &= 0x7FE0;
                treg |= value >> 3;
                xreg = value & 7;
            }else{
                treg &= 0xC1F;
                treg |= (value & 7) << 12;
                treg |= (value >> 3) << 5;
            }
            wreg ^= 1;
        }    
    }
}
void PPU::PPUADDR(){
    if(is_read) retVal = openbus;
    else{
        if(bus->getCycles() >= 29658){
            if(wreg == 0){
                regs[7] = value; //high byte
                treg &= 0xFF;
                treg |= (value << 8) & 0x3F00;
            } else{
                regs[8] = value; //low byte
                treg &= 0x7F00;
                treg |= value;
                // evaluateScrollX(); this is expected, but I will test.
                // evaluateScrollY();
            }
            wreg ^= 1;
        }
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
            buffer = VRAM[address];
        }else if(address < 0x3000){
            buffer = VRAM[address];
        }else if(address < 0x3F00){
            buffer = VRAM[address & 0x2FFF]; //mirror
        }else{
            buffer = VRAM[address & 0x2FFF];
            retVal = VRAM[address & 0x3F1F];
            // if(!(address & 3)) retVal = buffer = VRAM[address & 0x3F0F];
            // else retVal = buffer = VRAM[address & 0x3F1F];
        }
        bus_set = bus->getCycles();
        openbus = retVal;
    }else{
        if(address < 0x2000){
            // bus->writeAddress(0x6000 + address, value);
            printf("here\n");
            if(ram) VRAM[address] = value;
            //not used yet.
        }else if(address < 0x3000){
            VRAM[address] = value;
        }else if(address < 0x3F00){
            VRAM[address & 0x2FFF] = value;
        }else{
            if(!(address & 3)) VRAM[address & 0x3F0F] = value & 0x3F;
            else VRAM[address & 0x3F1F] = value & 0x3F;
        }
    }


    unsigned char nv = regs[8] + inc;
    if(nv < regs[8]) regs[7]++;
    regs[8] = nv; 
}


void PPU::setVblank(){
    if(regs[0] & 0x80){
        bus->setNMI();
    }
    regs[2] |= 0x80;
}

void PPU::clearVblank(){
    regs[2] &= ~0x20;
    regs[2] &= ~0x40;
    regs[2] &= ~0x80;
}

void PPU::move(){
    ++xx;
    if(xx == 341){
        xx = 0;
        yy++;
    }
    if(yy == 262){
        yy = 0;
    }
    if(xx < 256 && yy < 240){
        if(xx == 0) evaluateScrollX();
        if(yy == 0) evaluateScrollY();
        int cx = xx + sx;
        int cy = yy + sy;
        if(cx >= 512) cx -= 512;
        if(cy >= 480) cy -= 480;
        unsigned short bg = nametables[cx][cy];
        if(xx < 8 && (!(regs[1] & 0x2))) bg = 0x3F00;
        framebuffer[xx][yy] = color_pallete_1[VRAM[bg]];
        if(bg != 0x3F00 && sprzr[xx][yy]){
            regs[2] |= 0x40;
        }
        for(auto [cl, hid] : spr[xx][yy]){
            if(hid){
                if(bg == 0x3F00 && cl != 0x3F00) framebuffer[xx][yy] = color_pallete_1[VRAM[cl]];
            }else{
                if(cl != 0x3F00) framebuffer[xx][yy] = color_pallete_1[VRAM[cl]];
            }
        }
    }

    if(yy == 241 && xx == 1){
        setVblank();
    }
    if(yy == 261 && xx == 1){
        clearVblank();
        evaluateNametables();
        evaluateSprites();
        if(bus_set + 50000 < bus->getCycles()) openbus = 0;
        okVblank = true;
    }
    if(yy == 261 && xx == 339){
        if(even){
            yy = 0;
            xx = -1;
        }
        even ^= 1;
    }
    if(yy == 261 && xx >= 257 && xx <= 320) regs[3] = 0;
}


//Not perfect, but ok;
void PPU::evaluateNametables(){
    if(regs[1] & 0x08){
        if(horizontal){
            for(int j=0x2000;j<0x2400;j++){
                changeNametables(j, VRAM[j]);
            }
            for(int j=0x2800;j<0x2C00;j++){
                changeNametables(j, VRAM[j]);
            }
        }else{
            for(int j=0x2000;j<0x2800;j++){
                changeNametables(j, VRAM[j]);
            }
        }
    }else{
        for(int x=0;x<512;x++){
            for(int y=0;y<480;y++){
                nametables[x][y] = 0x3F00;
            }
        }
    }
}

void PPU::evaluateScrollX(){
    sx = ((treg & 0x1F) << 3) + (xreg & 7);
    if(treg & 0x400) sx += 256;
}

void PPU::evaluateScrollY(){
    sy = (((treg & 0x3E0) >> 5) << 3) + ((treg >> 12) & 7);
    if(treg & 0x800) sy += 240;
}

void PPU::changeNametables(unsigned short address, unsigned char value){
    if((address & 0x3FF) >= 0x3C0){
        return;
    }
    int rx = 8 * (address % 32);
    int ry = 8 * ((address & 0x3FF) / 32);
    unsigned short memplc = (address & 0x2C00) + 0x3C0 + (rx/32) + 8*(ry/32);
    unsigned char color_pat = VRAM[memplc];
    if((ry % 32) >= 16){
        if((rx % 32) >= 16) color_pat = (color_pat >> 6) & 3;
        else color_pat = (color_pat >> 4) & 3;
    }else{
        if((rx % 32) >= 16) color_pat = (color_pat >> 2) & 3;
        else color_pat = color_pat & 3;
    }
    rx += address & 0x400 ? 256 : 0;
    ry += address & 0x800 ? 240 : 0;
    
    int offset = 16 * value;
    if(regs[0] & 0x10) offset += 0x1000;
    
    
    for(int j=0;j<8;j++){
        unsigned char bytesl = VRAM[offset+j]; 
        unsigned char bytesr = VRAM[offset+j+8];
        for(int k=0; k < 8; k++){
            unsigned curColor = (bytesl & (1 << (7-k)))? 1 : 0;
            curColor += (bytesr & (1 << (7-k)))? 2 : 0;
            if(horizontal){
                int rf = rx >= 256 ? rx - 256 : rx + 256;
                nametables[rf + k][ry + j] = nametables[rx + k][ry + j] = curColor ? 0x3F00+color_pat*4 + curColor : 0x3F00;
            }else{
                int rf = ry >= 240 ? ry - 240 : ry + 240;
                nametables[rx + k][rf + j] = nametables[rx + k][ry + j] = curColor ? 0x3F00+color_pat*4 + curColor : 0x3F00;
            }
        }
    }    
}

void PPU::evaluateSprites(){
    for(int x=0;x<256;x++){
        for(int y=0;y<240;y++){
            sprzr[x][y] = false;
            spr[x][y].clear();
        }
    }
    if(!(regs[1] & 0x10)) return;
    bool mode = regs[0] & 0x20 ? true : false;//true means 8x16, false means 8x8
    unsigned short table = (regs[0] & 8)  && (!mode) ? 0x1000 : 0; 
    for(int i=63;i>=0;i--){
        unsigned char y = OAM[4*i];
        unsigned char x = OAM[4*i + 3];
        bool hidden = OAM[4*i + 2] & 0x20 ? true : false;
        bool flipx = OAM[4*i + 2] & 0x40 ? true : false;
        bool flipy = OAM[4*i + 2] & 0x80 ? true : false;
        int color_pat = OAM[4*i + 2] & 3;
        colors[1] = color_pallete_1[VRAM[(0x3F10+color_pat*4 + 1) & 0x3F1F]];
        colors[2] = color_pallete_1[VRAM[(0x3F10+color_pat*4 + 2) & 0x3F1F]];
        colors[3] = color_pallete_1[VRAM[(0x3F10+color_pat*4 + 3) & 0x3F1F]];
        unsigned short offset = table; 

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
            unsigned char bytesl, bytesr;
            if(j < 8){
                bytesl = VRAM[offset+j]; 
                bytesr = VRAM[offset+j+8];
            }else{
                bytesl = VRAM[offset+8+j]; 
                bytesr = VRAM[offset+16+j];
            }
            for(int k=0;k<8;k++){
                unsigned curColor = (bytesl & (1 << (7-k)))? 1 : 0;
                curColor += (bytesr & (1 << (7-k)))? 2 : 0;
                int rx = flipx ? x + 7 - k : x + k;
                int ry = ysz == 8 ? (flipy ? y + 7 - j: y + j) : (flipy ? y + 15 - j : y + j);
                if(rx >= 256 || ry >= 240 || curColor == 0) continue;
                if(rx < 8 && (!(regs[1] & 4))){
                    continue;
                }
                if(curColor != 0){
                    spr[rx][ry].push_back({0x3F10+color_pat*4 + curColor, hidden});
                }
                if(i == 0 && curColor != 0&& rx != 255 && ry != 239){
                    sprzr[rx][ry] = true;
                }
            }
        }
    
    }
}

void PPU::setRAM(){
    ram = true;
}

void PPU::printNametables(){
    Image img;
    img.makeImage(512, 480);
    for(int y=0;y<480;y++){
        for(int x = 0; x < 512; x++){
            Color c = color_pallete_1[VRAM[nametables[x][y]]];
            img.setPixel(x, y, c);
        }
    }
    img.writeImage("debug.bmp");
}