#include "ppu.h"
#include <cstdio>
#include <cstring>
#include <cassert>
#include <fstream>

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

Color color_pallete[8][64];

void initColorPallete(){
    std::ifstream input("includes/2C02G_wiki.pal", std::ios::binary);
    unsigned char *data = new unsigned char[8*64*3];
    input.read((char *)data, 8*64*3);
    for(int i=0;i<8;i++){
        for(int j=0;j<64;j++){
            color_pallete[i][j] = {data[i * 192 + 3 * j], data[i * 192 + 3 * j + 1], data[i * 192 + 3 * j + 2]};
        }
    }
}

PPU::PPU(){
    initColorPallete();
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
    else{ 
        if(((regs[0] & 0x80) == 0) && (value & 0x80) && (regs[2] & 0x80)){
            bus->cpu->delay_nmi = true;
        }
        regs[0] = value;
        treg &= 0x73FF;
        treg |= ((value & 3) << 10) & 0xC00;
    }
}   
void PPU::PPUMASK(){
    if(is_read) retVal = openbus;
    else regs[1] = value;
}
void PPU::PPUSTATUS(){
    if(is_read){
        openbus = retVal = regs[2] | (0x1F & openbus);
        bus_set = bus->cpu->total_cycles;
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
        bus_set = bus->cpu->total_cycles;
    }else{
        writeOAM(value);
    }
}
void PPU::PPUSCROLL(){
    if(is_read) retVal = openbus;
    else{
        if(wreg == 0){
            treg &= 0x7FE0;
            treg |= value >> 3;
            sx -= xreg;
            xreg = value & 7;
            sx += xreg; //fine x update inside line.
        }else{
            treg &= 0xC1F;
            treg |= (value & 7) << 12;
            treg |= (value >> 3) << 5;
        }
        wreg ^= 1;
    }
    // evaluateScrollX();
}
void PPU::PPUADDR(){
    if(is_read) retVal = openbus;
    else{
        // printf("addr %d %d, w = %d, a = %02X\n", yy, xx, wreg, value);
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
            vreg = treg;
            evaluateScroll();
        }
        wreg ^= 1;
    }
}




void PPU::PPUDATA(){
    // printf("wr %d %d == %04X  %02X\n", yy, xx, vreg, value);
    int inc = regs[0] & 0x4 ? 32 : 1;
    unsigned short addr = vreg & 0x3FFF;
    if(is_read){
        retVal = buffer;
        if(addr < 0x2000){
            buffer = bus->readCartridge(addr);
        }else if(addr < 0x3F00){
            addr &= 0xFFF;
            addr = getAddress(addr);
            buffer = VRAM[addr];
        }else{
            buffer = VRAM[getAddress(addr & 0xFFF)];
            retVal = (pmem[addr & 0x1F] & 0x3F) | (openbus & 0xC0);
        }
        bus_set = bus->cpu->total_cycles;
        openbus = retVal;
    }else{
        if(addr < 0x2000){
            bus->writeCartridge(addr, value);
        }else if(addr < 0x3F00){
            addr &= 0xFFF;
            addr = getAddress(addr);
            VRAM[addr] = value;
        }else{
            if(!(addr & 3)) pmem[addr & 0xF] = pmem[0x10 | (addr & 0xF)] = value & 0x3F;
            else pmem[addr & 0x1F] = value & 0x3F;
        }
    }
    int ob = vreg & 0x1000;
    vreg += inc;
    // vreg %= 0x4000;
    if((vreg & 0x1000) == 0x1000 && ob == 0) bus->cartridge->Clockmm3();
    evaluateScroll();
}


void PPU::setVblank(){
    if(regs[0] & 0x80){
        bus->cpu->nmi_pin = true;
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
        if(xx == 0) {
            evaluateSprites(yy); //this is wrong, but easy to fix?!
        }
        int cx = sx;
        int cy = sy;
        if(cx >= 512) cx -= 512;
        if(cy >= 480) cy -= 480;
        //Background
        unsigned short address = 0;
        if(cx >= 256) address += 0x400;
        if(cy >= 240) address += 0x800;
        address = getAddress(address);
        cx %= 256, cy %= 240;
        int memplc = address + 0x3C0 + (cx/32) + 8*(cy/32);
        int bit;
        if((cy % 32) >= 16){
            if((cx % 32) >= 16) bit = 6;
            else  bit = 4;
        }else{
            if((cx % 32) >= 16) bit = 2;
            else bit = 0;
        }
        address += (cx & 0xFF) >> 3;
        address += (cy >> 3) << 5;
        unsigned short offset = 16 * VRAM[address];
        if((regs[0] & 0x10)) offset += 0x1000;
        int jj = cy & 7;
        int kk = cx & 7;
        unsigned char bytesl = bus->readCartridge(offset+jj); 
        unsigned char bytesr = bus->readCartridge(offset+jj+8);
        int cl = ((bytesl & (1 << (7-kk)))? 1 : 0) + ((bytesr & (1 << (7-kk)))? 2 : 0);
        int pal = (VRAM[memplc] >> bit) & 3;
        unsigned short bg = cl ?  pal * 4 + cl : 0;
        if(xx < 8 && (!(regs[1] & 0x2))) bg = 0;
        if(!(regs[1] & 8)) bg = 0;

 
        //Sprites
        if(bg != 0 && sprzr[xx] && yy <= 239){
            regs[2] |= 0x40; 
        }
        unsigned short res = bg;
        for(auto [cl, hid] : spr[xx]){
            if(cl != 0){
                if(hid && bg != 0) res = bg;
                else res = cl;
            }
        }
        unsigned char color = pmem[res];
        if(regs[1] & 1) color &= 0x30;
        framebuffer[xx][yy] = color_pallete[regs[1] >> 5][color];
        sx++;
    }
    
    if(xx == 256 && (yy < 240 || yy == 261) && (regs[1] & 0x18)){
        int val = (0x7000 & vreg) >> 12;
        if(val == 7){
            vreg &= 0x0FFF;
            val = (vreg & 0x3E0) >> 5;
            if(val == 29){
                val = 0;
                vreg ^= 0x800;
                if(vreg & 0x800) sy = 240;
                else sy = 0;
            }else if(val == 31){
                if(vreg & 0x800) sy = 240;
                else sy = 0;
                val = 0;
            }else{
                sy++;
                val += 1;
            }
            vreg &= ~ 0x3E0;
            vreg |= val << 5;
        }else{
            vreg += 0x1000;
            sy++;
        }
        
    }
    if(yy == 241 && xx == 1){
        setVblank();
    }
    if(yy == 261 && xx == 1){
        clearVblank();
        if(bus_set + 50000 < bus->cpu->total_cycles) openbus = 0;
        okVblank = true;
        if(regs[1] & 0x18) vreg = treg;
    }
    if(xx == 260 && (yy < 240 || yy == 261)){ //not correct but working.
        if((regs[1] & 0x18)) bus->cartridge->Clockmm3();
    }
    if(yy == 261 && xx == 339){
        if(even){
            yy = 0;
            xx = -1;
        }
        even ^= 1;
    }
    if(yy == 261 && xx >= 280 && xx <= 304){
        if(regs[1] & 0x18) evaluateScrollY();
        else sy = 0;
    }
    if(xx == 257 && (yy < 240 || yy == 261)){
        if((regs[1] & 0x18)) evaluateScrollX();
        else sx = 0;
    }
    if(yy == 261 && xx >= 257 && xx <= 320) regs[3] = 0;
}

//no copy treg to vreg here
void PPU::evaluateScroll(){
    sy = (((vreg & 0x3E0) >> 5) << 3) + ((vreg >> 12) & 7);
    if(vreg & 0x800) sy += 240;
    sy %= 480;
    sx = ((vreg & 0x1F) << 3) + (xreg & 7);
    if(vreg & 0x400) sx += 256;
}
void PPU::evaluateScrollY(){
    vreg &= ~0x7BE0;
    vreg |= 0x7BE0 & treg;
    sy = (((vreg & 0x3E0) >> 5) << 3) + ((vreg >> 12) & 7);
    if(vreg & 0x800) sy += 240;
    sy %= 480;
}
//sometimes it changes mid screen
void PPU::evaluateScrollX(){
    vreg &= ~0x41F;
    vreg |= 0x41F & treg;
    sx = ((vreg & 0x1F) << 3) + (xreg & 7);
    if(vreg & 0x400) sx += 256;
}

void PPU::evaluateSprites(int yy){
    //Once per line...
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
        unsigned char y = OAM[addr] + 1;
        if(!y) continue;
        unsigned char x = OAM[addr + 3];
        if(y > yy || y + ysz - 1 < yy) continue;
        bool hidden = OAM[addr + 2] & 0x20 ? true : false;
        bool flipx = OAM[addr + 2] & 0x40 ? true : false;
        bool flipy = OAM[addr + 2] & 0x80 ? true : false;
        int color_pat = OAM[addr + 2] & 3;
        
        int j = yy - y;
        int ry = ysz == 8 ? (flipy ? y + 7 - j: y + j) : (flipy ? y + 15 - j : y + j);
        j = ry - y;
        unsigned char bytesl, bytesr;
        if(j < 8){
            bytesl = bus->cartridge->readMemory(offset+j);
            bytesr = bus->cartridge->readMemory(offset+j+8);
        }else{
            bytesl = bus->cartridge->readMemory(offset+8+j); 
            bytesr = bus->cartridge->readMemory(offset+16+j);
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
                spr[rx].push_back({0x10+color_pat*4 + curColor, hidden});
            }
            
            if(i == 0 && curColor != 0&& rx != 255 && y != 240){
                sprzr[rx] = true;
            }
        }
    }
}


void PPU::powerON(){
    okVblank = false;
    xx = yy = sx = sy = 0;
    memset(VRAM, 0, sizeof(VRAM));
    memset(pmem, 0, sizeof(pmem));
    memset(regs, 0, sizeof(regs));
    memset(OAM, 0, sizeof(OAM));
}

//address from 0 to 0xFFF based on mirroring
unsigned short PPU::getAddress(unsigned short addr){
    switch(mirror){
        case 0:
            addr &= ~0x400;
            if(addr >= 0x800) addr -= 0x400;
            break;
        case 1: 
            addr &= ~0x800;
            break;
        case 2:
            //do nothing
            break;
        case 3:
            addr &= 0x3FF;
            break;
    }
    return addr;
}