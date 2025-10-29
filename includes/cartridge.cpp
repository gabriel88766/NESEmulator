//This class will be used to mappers.
//DEFAULT 8KB VRAM from 0x6000 to 0x8000
#include "cartridge.h"
#include <fstream>
#include <cassert>
#include <cstring>

Cartridge::~Cartridge(){
    delete[] header;
    delete[] prg_banks;
    delete[] chr_banks;
    delete[] prg_ram; 
    delete[] prg_rom;
    delete[] chr_rom;
}

void Cartridge::connectBus(Bus *bus){
    this->bus = bus;
}

bool Cartridge::read(const char *filename){
    std::ifstream input(filename, std::ios::binary);
    unsigned char nheader[16];
    input.read( (char*)nheader, 16 );
    if(nheader[0] == 'N' && nheader[1] == 'E' && nheader[2] == 'S' && nheader[3] == 0x1A){
        delete[] header;
        header = new unsigned char[16];
        memcpy(header, nheader, 16);
        mapper = (header[7] & 0xF0) | (header[6] >> 4);
        printf("mapped %d\n", mapper);
        if(header[6] & 1){
            bus->ppu->mirror = bus->ppu->VERTICAL;
        }else{
            bus->ppu->mirror = bus->ppu->HORIZONTAL;
        }
        delete[] trainer;
        delete[] prg_banks;
        delete[] chr_banks;
        delete[] prg_ram; 
        delete[] prg_rom;
        delete[] chr_ram;
        delete[] chr_rom;
        I = false;
        // if(header[6] & 2) ram = true;
        prg_banks = new unsigned char[0x4000 * header[4]];
        prg_rom = new unsigned char*[8];
        prg_ram = new unsigned char[0x8000];
        chr_rom = new unsigned char*[8];
        if(header[6] & 0x04){
            trainer = new unsigned char[0x200];
            input.read((char*)trainer, 0x200);
        }
        input.read((char *)prg_banks, 0x4000 * header[4]);
        if(header[5] == 0){
            if(mapper == 7 || mapper == 11){
                header[5] = 1;
            }else{
                header[5] = 0xFF;
            }
            ram = true;
            chr_banks = new unsigned char[0x512000]; //huge chunk of ram, enough for any game ~5MB
            for(int i=0;i<8;i++) chr_rom[i] = chr_banks + 0x400 * i;
        }else{
            ram = false;
            chr_banks = new unsigned char[0x2000 * header[5]];
            input.read((char *)chr_banks, 0x2000 * header[5]);
            for(int i=0;i<8;i++) chr_rom[i] = chr_banks + 0x400 * i;
        }

        if(header[4] >= 2){
            loadPrgLB();
        }else if(header[4] == 1){
            for(int i=0;i<4;i++) prg_rom[i] = prg_banks + 0x1000 * i;
            for(int i=4;i<8;i++) prg_rom[i] = prg_banks + 0x1000 * (i - 4);
        }
        switch(mapper){
            case 1:
                ctrl1 = 0xC;
                break;
            case 4:
                reload = chr = b2k = irq4 = false;
                cnt4 = rlirq4 = 0;
                //Rad Racer II
                if(header[6] & 0x08){ bus->ppu->mirror = bus->ppu->FOUR_SCREEN; }
                break;
        }
        return true;
    }else{
        return false;
    }


    
}



unsigned char Cartridge::readMemory(unsigned short address){
    if(address <= 0x1FFF){
        return chr_rom[address >> 10][address & 0x3FF];
    }
    if(address >= 0x4020 && address < 0x6000){
        switch(mapper){
            case 243:{
                return reg[creg];
            }
        }
    }
    if(address >= 0x8000){
        return prg_rom[(address & 0x7FFF) >> 12][(address & 0xFFF)];
    }
    if(address >= 0x6000 && address < 0x8000){
        return prg_ram[address & 0x1FFF];
    }
    return 0; //never should happen
}
void Cartridge::writeMemory(unsigned short address, unsigned char value){
    if(address <= 0x1FFF){
        if(ram){
            chr_rom[address >> 10][address & 0x3FF] = value;
        }
    }else if(address >= 0x4020 && address < 0x6000){
        switch(mapper){
            case 79:{
                //addres & 0x4100 == 0x4100
                axromPrg((value & 0x8) >> 3);
                cxromChr(value & 7);
                break;
            }
            case 243:{
                if(address == 0x4100) creg = value & 7;
                if(address == 0x4101){
                    reg[creg] = value & 7;
                    switch(creg){
                        case 2:
                            chr_addr &= ~0x10000;
                            chr_addr |= (value & 1) << 16;
                            break;
                        case 4:
                            chr_addr &= ~0x8000;
                            chr_addr |= (value & 1) << 15;
                            break;
                        case 5:{
                            int x = value & 3;
                            if(header[4] <= 4) x &= 1;
                            for(int j=0;j<8;j++) prg_rom[j] = prg_banks + 0x8000 * x + 0x1000 * j;
                            break;
                        }
                        case 6:
                            chr_addr &= ~0x6000;
                            chr_addr |= (value & 3) << 13;
                            break;
                        case 7:{
                            int x = (value & 0x6) >> 1;
                            if(x == 0){
                                printf("Need to implement mapper 243 nametable type = 0\n");
                                //Not implemented yet.
                            }else if(x == 1){
                                bus->ppu->mirror = bus->ppu->HORIZONTAL;
                            }else if(x == 2){
                                bus->ppu->mirror = bus->ppu->VERTICAL;
                            }else{
                                bus->ppu->mirror = bus->ppu->SINGLE_SCREEN_UP;
                            }
                            break;
                        }
                    }
                    chr_addr %= header[5] * 0x2000;
                    for(int j=0;j<8;j++) chr_rom[j] = chr_banks + chr_addr  + 0x400 * j;
                }
                break;
            }
        }
    }else if(address >= 0x6000 && address < 0x8000){
        prg_ram[address & 0x1FFF] = value;
        switch(mapper){
            case 87:{
                unsigned char rv = value & 1 ? 2 : 0;
                rv |= value & 2 ? 1 : 0;
                cxromChr(rv);
                break;
            }
            case 184:{
                unsigned char high = (value & 0x70) >> 4;
                unsigned char low = (value & 7);
                if(header[5] == 1) low &= 1, high &= 1;
                else if(header[5] <= 2) low &= 3, high &= 3;
                for(int i=0;i<4;i++) chr_rom[i] = chr_banks + low * 0x1000 + 0x400*i;
                for(int i=0;i<4;i++) chr_rom[i+4] = chr_banks + high * 0x1000 + 0x400 * i;
                break;
            }
        }
    }else if(address >= 0x8000){
        //bankswitch?
        switch(mapper){
            case 1:{
                if(value & 0x80){
                    reg1 = 0;
                    for(int i=4;i<8;i++) prg_rom[i] = prg_banks + (header[4] - 1) * 0x4000 + 0x1000 * (i-4);
                    ctrl1 |= 0xC;
                }else{
                    unsigned char val = reg1 & 7;
                    if(value & 1){
                        reg1 |= 1 << (val + 3);
                        
                    }
                    reg1++;
                    if(val == 4){
                        //then reg1 is ready
                        unsigned char rv = (reg1 & 0xF8) >> 3;
                        if(address <= 0x9FFF){
                            ctrl1 = rv;
                            int v1 = ctrl1 & 3;
                            if(v1 == 0) bus->ppu->mirror = bus->ppu->SINGLE_SCREEN_DOWN;
                            else if(v1 == 1) bus->ppu->mirror = bus->ppu->SINGLE_SCREEN_UP;
                            else if(v1 == 2) bus->ppu->mirror = bus->ppu->VERTICAL;
                            else bus->ppu->mirror = bus->ppu->HORIZONTAL;
                            int v2 = (ctrl1 >> 2) & 3;
                            if(v2 == 2){
                                for(int i=0;i<4;i++) prg_rom[i] = prg_banks + 0x1000 * i;
                            }else if(v2 == 3){
                                for(int i=4;i<8;i++) prg_rom[i] = prg_banks + (header[4] - 1) * 0x4000 + 0x1000 * (i-4);
                            }
                        }else if(address <= 0xBFFF){
                            int v1 = ctrl1 & 0x10;
                            rv %= 2*header[5];
                            if(v1){
                                for(int i=0;i<4;i++) chr_rom[i] = chr_banks + rv * 0x1000 + 0x400 * i;
                            }else{
                                rv &= 0x1E;
                                
                                for(int i=0;i<8;i++) chr_rom[i] = chr_banks + rv * 0x1000 + 0x400 * i;
                            }
                        }else if(address <= 0xDFFF){
                            rv %= 2*header[5];
                            int v1 = ctrl1 & 0x10;
                            if(v1){
                                for(int i=0;i<4;i++) chr_rom[i+4] = chr_banks + rv * 0x1000 + 0x400 * i;
                            }
                        }else{
                            int v1 = (ctrl1 >> 2) & 3;
                            rv %= header[4];
                            if(v1 == 0 || v1 == 1){
                                rv &= 0x0E;
                                for(int i=0;i<8;i++) prg_rom[i] = prg_banks + rv * 0x4000 + 0x1000 * i;
                            }else{
                                rv &= 0x0F;
                                if(v1 == 2){
                                    for(int i=0;i<4;i++) prg_rom[i+4] = prg_banks + rv * 0x4000 + 0x1000 * i;
                                }else{
                                    for(int i=0;i<4;i++) prg_rom[i] = prg_banks + rv * 0x4000 + 0x1000 * i;
                                }
                            }
                        }
                        reg1 = 0;
                    }
                }
                break;
            }
            case 2:{
                uxromPrg(value);
                break;
            }
            case 3:{
                cxromChr(value);
                break;
            }
            case 4:{
                if(address % 2 == 0 && address <= 0x9FFE){
                    int j;
                    if((value & 7) >= 6){
                        chr = false;
                        if((value & 7) == 7) nb = 2;
                        else nb = 0;
                        if(value & 0x40){
                            if(nb == 0) nb += 4;
                            for(int i=0;i<2;i++) prg_rom[i] = prg_banks + (header[4] - 1) * 0x4000 + 0x1000 * i;
                        }else{
                            for(int i=0;i<2;i++) prg_rom[i+4] = prg_banks + (header[4] - 1) * 0x4000 + 0x1000 * i;
                        }
                    }else{
                        chr = true;
                        b2k = false;
                        switch(value & 7){
                            case 0:
                                nb = 0;
                                b2k = true;
                                break;
                            case 1:
                                nb = 2;
                                b2k = true;
                                break;
                            case 2:
                                nb = 4;
                                break;
                            case 3:
                                nb = 5;
                                break;
                            case 4:
                                nb = 6;
                                break;
                            case 5:
                                nb = 7;
                                break;
                        }
                        if(value & 0x80) nb ^= 4;
                    }
                }
                if(address % 2 == 1 && address <= 0x9FFE){
                    if(chr){
                        if(value >= 8 * header[5]){
                            value %= (8 * header[5]); //Pac-Mania
                        }
                        if(b2k){
                            value &= 0xFE;
                            for(int i=0;i<2;i++){
                                chr_rom[nb + i] = chr_banks + 0x400 * value + 0x400 * i;
                            }
                        }else{
                            chr_rom[nb] = chr_banks + 0x400 * value;
                        }
                    }else{
                        value &= 0x3F;
                        if(value >= 2*header[4]){
                            value %= (2*header[4]); //Super Contra?
                        }
                        for(int i=0;i<2;i++){
                            prg_rom[nb + i] = prg_banks + 0x2000 * value + 0x1000 * i;
                        }
                        
                    }
                    
                }   
                if(address % 2 == 0 && address >= 0xA000 && address <= 0xBFFF){
                    if(!(header[6] & 0x08)){
                        if(value & 1) bus->ppu->mirror = bus->ppu->HORIZONTAL;
                        else bus->ppu->mirror = bus->ppu->VERTICAL;
                    }
                }
                if(address % 2 == 0 && address >= 0xC000 && address <= 0xDFFF){
                    rlirq4 = value;
                }
                if(address % 2 == 1 && address >= 0xC000 && address <= 0xDFFF){
                    cnt4 = 0;
                    reload = true;
                }
                if(address % 2 == 0 && address >= 0xE000) {
                    irq4 = I = false;
                }
                if(address % 2 == 1 && address >= 0xE000){
                    irq4 = true;
                }
                break;
            }
            case 7:{
                axromPrg(value & 7);
                bus->ppu->mirror = value & 0x10 ? bus->ppu->SINGLE_SCREEN_UP :  bus->ppu->SINGLE_SCREEN_DOWN;
                break;
            }
            case 11:{
                axromPrg(value & 3);
                cxromChr((value & 0xF0) >> 4);
                break;
            }
            case 71:{
                if(address == 0x9000) bus->ppu->mirror = value & 0x10 ? bus->ppu->SINGLE_SCREEN_UP :  bus->ppu->SINGLE_SCREEN_DOWN;
                if(address >= 0xC000) uxromPrg(value);
                break;
            }
        }
    }
}

void Cartridge::Clockmm3(){
    if(mapper != 4) return;
    if(reload){
        reload = I = false;
        cnt4 = rlirq4;
    }else if(cnt4 != 0){
        cnt4--;
        if(cnt4 == 0 && irq4 == true) I = true;
    }else{
        cnt4 = rlirq4;
        if(irq4 && cnt4 == 0) I = true;
    }
}

void Cartridge::loadPrgLB(){
    for(int i=0;i<4;i++) prg_rom[i] = prg_banks + 0x1000 * i;
    for(int i=4;i<8;i++) prg_rom[i] = prg_banks + (header[4] - 1) * 0x4000 + 0x1000 * (i-4);
}

void Cartridge::uxromPrg(unsigned char value){
    int val;
    if(header[4] <= 2) val = value & 1;
    else if(header[4] <= 4) val = value & 3;
    else if(header[4] <= 8) val = value & 7;
    else if(header[4] <= 16) val = value & 15;
    for(int i=0;i<4;i++) prg_rom[i] = prg_banks + val * 0x4000 + 0x1000 * i;
}

void Cartridge::axromPrg(unsigned char value){
    int val;
    if(header[4] == 2) val = 0;
    else if(header[4] <= 4) val = value & 1;
    else if(header[4] <= 8) val = value & 3;
    else if(header[4] <= 16) val = value & 7;
    else val = value & 0xF;
    for(int i=0;i<8;i++)  prg_rom[i] = prg_banks + 0x1000 * i + 0x8000 * val;
}

void Cartridge::cxromChr(unsigned char value){
    int val;
    if(header[5] == 1) val = 0;
    else if(header[5] <= 2) val = value & 1;
    else if(header[5] <= 4) val = value & 3;
    else if(header[5] <= 8) val = value & 7;
    else val = value & 0xF;
    for(int i=0;i<8;i++) chr_rom[i] = chr_banks + val * 0x2000 + 0x400 * i;
}