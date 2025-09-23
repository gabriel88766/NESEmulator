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
    powerON();
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
    if(is_read){
        openbus = retVal = regs[2] | (0x1F & openbus);
        bus_set = bus->getCycles();
        regs[2] &= 0x7F;
        wreg = 0;
    }else openbus = value;
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
            printf("write : wreg = %d, value = %2X\n", wreg, value);
            if(wreg == 0){
                int ob = regs[7] & 0x10;
                regs[7] = value; //high byte
                treg &= 0xFF;
                treg |= (value << 8) & 0x3F00;
                if((regs[7] & 0x10) == 0x10 && ob == 0) bus->cartridge->Clockmm3();
            } else{
                regs[8] = value; //low byte
                treg &= 0x7F00;
                treg |= value;
                evaluateScrollY();
            }
            wreg ^= 1;
        }
    }
}
void PPU::PPUDATA(){
    int inc = regs[0] & 0x4 ? 32 : 1;
    unsigned short address = regs[7];
    address <<= 8;
    address |= regs[8];
    address &= 0x3FFF;
    if(is_read){
        retVal = buffer;
        if(address < 0x2000){
            buffer = bus->readCartridge(address);
        }else if(address < 0x3000){
            buffer = VRAM[address];
            
        }else if(address < 0x3F00){
            buffer = VRAM[address & 0x2FFF]; //mirror
        }else{
            buffer = VRAM[address & 0x2FFF];
            retVal = (VRAM[address & 0x3F1F] & 0x3F) | (openbus & 0xC0);
        }
        bus_set = bus->getCycles();
        openbus = retVal;
    }else{
        if(address < 0x2000){
            bus->writeCartridge(address, value);
        }else if(address < 0x3000){
            if(horizontal){
                VRAM[address ^ 0x400] = VRAM[address] = value;
            }else{
                VRAM[address ^ 0x800] = VRAM[address] = value;
            }
        }else if(address < 0x3F00){
            address &= 0x2FFF;
            if(horizontal){
                VRAM[address ^ 0x400] = VRAM[address] = value;
            }else{
                VRAM[address ^ 0x800] = VRAM[address] = value;
            }
        }else{
            
            if(!(address & 3)) VRAM[address & 0x3F0F] = VRAM[0x10 | (address & 0x3F0F)] = value & 0x3F;
            else VRAM[address & 0x3F1F] = value & 0x3F;
        }
    }

    int ob = regs[7] & 0x10;
    unsigned char nv = regs[8] + inc;
    if(nv < regs[8]) regs[7]++;
    regs[8] = nv; 
    if((regs[7] & 0x10) == 0x10 && ob == 0) bus->cartridge->Clockmm3();

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
        sy++;
    }
    if(yy == 262){
        printf("\n");
        yy = 0;
    }
    if(xx < 256 && yy < 240){
        if(xx == 0) {evaluateScrollX(), evaluateSprites(yy);}
        if(xx == 0 && yy == 0){
            evaluateScrollY();
        }
        int cx = xx + sx;
        int cy = sy;
        if(cx >= 512) cx -= 512;
        if(cy >= 480) cy -= 480;
        //Background
        unsigned short address = 0x2000;
        if(cx >= 256) address += 0x400;
        if(cy >= 240) address += 0x800;
        address += (cx & 0xFF) >> 3;
        address += ((cy >= 240 ? cy - 240 : cy) >> 3) << 5;
        unsigned short offset = 16 * VRAM[address];
        if((regs[0] & 0x10)) offset += 0x1000;
        int jj = cy & 7;
        int kk = cx & 7;
        unsigned char bytesl = bus->readCartridge(offset+jj); 
        unsigned char bytesr = bus->readCartridge(offset+jj+8);
        int cl = ((bytesl & (1 << (7-kk)))? 1 : 0) + ((bytesr & (1 << (7-kk)))? 2 : 0);
        int pal = (VRAM[mp[cx][cy]] >> mp2[cx][cy]) & 3;
        unsigned short bg = cl ?  0x3F00 + pal * 4 + cl : 0x3F00;
        if(xx < 8 && (!(regs[1] & 0x2))) bg = 0x3F00;
        if(!(regs[1] & 8)) bg = 0x3F00;

 
        //Sprites
        if(bg != 0x3F00 && sprzr[xx] && yy < 239){
            regs[2] |= 0x40; 
            // if(lzhit == 0) lzhit = 257;
        }
        if(lzhit){
            // lzhit--;
            // if(lzhit == 0){  regs[2] |= 0x40; }
        }
        unsigned short res = bg;
        for(auto [cl, hid] : spr[xx]){
            if(cl != 0x3F00){
                if(hid && bg != 0x3F00) res = bg;
                else res = cl;
            }
        }
        framebuffer[xx][yy] = color_pallete_1[VRAM[res]];
    }

    if(yy == 241 && xx == 1){
        setVblank();
    }
    if(yy == 261 && xx == 1){
        clearVblank();
        fillMaps();
        if(bus_set + 50000 < bus->getCycles()) openbus = 0;
        okVblank = true;
    }
    if(xx == 260 && (yy <= 240)){
        if((regs[1] & 0x18)) bus->cartridge->Clockmm3();
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

void PPU::evaluateScrollX(){
    sx = ((treg & 0x1F) << 3) + (xreg & 7);
    if(treg & 0x400) sx += 256;
}

void PPU::evaluateScrollY(){
    sy = (((treg & 0x3E0) >> 5) << 3) + ((treg >> 12) & 7);
    if(treg & 0x800) sy += 240;
    sy %= 480;
}

//maps to enhance performance.
void PPU::fillMaps(){
    for(unsigned short address = 0x2000; address <= 0x2FFF; address++){
        if((address & 0x3FF) >= 0x3C0) continue;
        int rx = 8 * (address % 32);
        int ry = 8 * ((address & 0x3FF) / 32);
        unsigned short memplc = (address & 0x2C00) + 0x3C0 + (rx/32) + 8*(ry/32);
        int bit = 0;
        if((ry % 32) >= 16){
            if((rx % 32) >= 16) bit = 6;
            else  bit = 4;
        }else{
            if((rx % 32) >= 16) bit = 2;
            else bit = 0;
        }

        rx += address & 0x400 ? 256 : 0;
        ry += address & 0x800 ? 240 : 0;
        for(int j=0;j<8;j++){
            for(int k=0; k < 8; k++){
                mp[rx + k][ry + j] = memplc;
                mp2[rx + k][ry + j] = bit;
            }
        }    
    }
}

void PPU::evaluateSprites(int yy){
    //Once per line...
    Color colors[4];
    for(int x=0;x<256;x++){
        sprzr[x] = false;
        spr[x].clear();
    }
    if(!(regs[1] & 0x10)) return;
    bool mode = regs[0] & 0x20 ? true : false;//true means 8x16, false means 8x8
    unsigned short table = (regs[0] & 8)  && (!mode) ? 0x1000 : 0; 
    for(int i=63;i>=0;i--){
        unsigned char addr = regs[3] + 4*i;
        unsigned short offset = table; 
        int ysz;
        if(mode){
            ysz = 16;
            if(OAM[addr + 1] & 1) offset += 0x1000;
            offset += 16 * (OAM[addr + 1] & 0xFE);
        }else{
            ysz = 8;
            offset += 16 * OAM[addr + 1]; 
        }
        
        unsigned char y = OAM[addr];
        unsigned char x = OAM[addr + 3];
        if(y > yy || y + ysz - 1 < yy) continue;

        bool hidden = OAM[addr + 2] & 0x20 ? true : false;
        bool flipx = OAM[addr + 2] & 0x40 ? true : false;
        bool flipy = OAM[addr + 2] & 0x80 ? true : false;
        int color_pat = OAM[addr + 2] & 3;
        colors[1] = color_pallete_1[VRAM[(0x3F10+color_pat*4 + 1) & 0x3F1F]];
        colors[2] = color_pallete_1[VRAM[(0x3F10+color_pat*4 + 2) & 0x3F1F]];
        colors[3] = color_pallete_1[VRAM[(0x3F10+color_pat*4 + 3) & 0x3F1F]];
        
        int j = yy - y;
        int ry = ysz == 8 ? (flipy ? y + 7 - j: y + j) : (flipy ? y + 15 - j : y + j);
        j = ry - y;
        unsigned char bytesl, bytesr;
        if(j < 8){
            bytesl = bus->readCartridge(offset+j);
            bytesr = bus->readCartridge(offset+j+8);
        }else{
            bytesl = bus->readCartridge(offset+8+j); 
            bytesr = bus->readCartridge(offset+16+j);
        }
        for(int k=0;k<8;k++){
            unsigned curColor = (bytesl & (1 << (7-k)))? 1 : 0;
            curColor += (bytesr & (1 << (7-k)))? 2 : 0;
            int rx = flipx ? x + 7 - k : x + k;
            if(rx >= 256 || ry >= 240 || curColor == 0) continue;
            if(rx < 8 && (!(regs[1] & 4))){
                continue;
            }
            if(curColor != 0){
                spr[rx].push_back({0x3F10+color_pat*4 + curColor, hidden});
            }
            
            if(i == 0 && curColor != 0&& rx != 255 && y != 239){
                sprzr[rx] = true;
            }
        }
    }
}


void PPU::powerON(){
    okVblank = false;
    xx = yy = sx = sy = 0;
    memset(VRAM, 0, sizeof(VRAM));
    memset(regs, 0, sizeof(regs));
    memset(OAM, 0, sizeof(OAM));
    memset(nametables0, 0, sizeof(nametables0));
    memset(nametables1, 0, sizeof(nametables1));
    memset(opaque, 0, sizeof(opaque));
    memset(isopaque, 0, sizeof(isopaque));
    memset(sprzr, 0, sizeof(sprzr));
    fillMaps();
}