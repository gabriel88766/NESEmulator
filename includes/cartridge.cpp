//This class will be used to mappers.
//DEFAULT 8KB VRAM from 0x6000 to 0x8000
#include "cartridge.h"
#include <fstream>
#include <cassert>

Cartridge::~Cartridge(){
    delete[] header;
    delete[] prg_banks;
    delete[] chr_banks;
    delete[] prg_ram; 
    delete[] prg_rom;
    delete[] chr_rom;
}

void Cartridge::load(unsigned char *dst, unsigned char *src, int sz){
    memcpy(dst, src, sz);
}

void Cartridge::connectBus(Bus *bus){
    this->bus = bus;
}

bool Cartridge::read(const char *filename){
    std::ifstream input(filename, std::ios::binary);
    delete[] header;
    header = new unsigned char[16];
    input.read( (char*)header, 16 );
    ram = false;
    if(header[0] = 'N' && header[1] == 'E' && header[2] == 'S' && header[3] == 0x1A){
        //TODO: assert iNes, this emulator isn't indented to support NES 2.0
        mapper = (header[7] & 0xF0) | (header[6] >> 4);
        if(header[6] & 1){
            bus->setPPUHorizontal(false);
        }else{
            bus->setPPUHorizontal(true);
        }
        delete[] prg_banks;
        delete[] chr_banks;
        delete[] prg_ram; 
        delete[] prg_rom;
        delete[] chr_ram;
        delete[] chr_rom;
        I = false;
        // if(header[6] & 2) ram = true;
        prg_banks = new unsigned char[0x4000 * header[4]];
        chr_banks = new unsigned char[0x2000 * header[5]];
        prg_rom = new unsigned char*[8];
        prg_ram = new unsigned char[0x2000];
        chr_rom = new unsigned char*[8];
        chr_ram = new unsigned char[0x2000];
        input.read((char *)prg_banks, 0x4000 * header[4]);
        input.read((char *)chr_banks, 0x2000 * header[5]);
        if(header[5] == 0){
            ram = true;
            for(int i=0;i<8;i++) chr_rom[i] = chr_ram + 0x400 * i;
        }else{
            //bank 0, standard.
            for(int i=0;i<8;i++) chr_rom[i] = chr_banks + 0x400 * i;
        }
        if(header[4] == 2){
            for(int i=0;i<8;i++) prg_rom[i] = prg_banks + 0x1000 * i;
        }else if(header[4] == 1){
            for(int i=0;i<4;i++) prg_rom[i] = prg_banks + 0x1000 * i;
            for(int i=4;i<8;i++) prg_rom[i] = prg_banks + 0x1000 * (i - 4);
        }
        switch(mapper){
            case 0:
                break;
            case 2:
                for(int i=0;i<4;i++) prg_rom[i] = prg_banks + 0x1000 * i;
                for(int i=4;i<8;i++) prg_rom[i] = prg_banks + (header[4] - 1) * 0x4000 + 0x1000 * (i-4);
                break;
            case 3:
                if(header[4] == 2){
                    for(int i=0;i<8;i++) prg_rom[i] = prg_banks + 0x1000 * i;
                }
                break;
            case 4:
                reload = chr = b2k = irq4 = false;
                cnt4 = rlirq4 = 0;
                for(int i=0;i<4;i++) prg_rom[i] = prg_banks + (header[4] - 1) * 0x4000 + 0x1000 * i;
                for(int i=4;i<8;i++) prg_rom[i] = prg_banks + (header[4] - 1) * 0x4000 + 0x1000 * (i-4);
                break;
            case 185:
                if(header[4] == 2){
                    for(int i=0;i<8;i++) prg_rom[i] = prg_banks + 0x1000 * i;
                }else if(header[4] == 1){
                    for(int i=0;i<4;i++) prg_rom[i] = prg_banks + 0x1000 * i;
                    for(int i=4;i<8;i++) prg_rom[i] = prg_banks + 0x1000 * (i - 4);
                }
                for(int i=0;i<8;i++) chr_rom[i] = chr_banks + 0x400 * i;
                break;
            default:
                return false;
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
    }else if(address >= 0x6000 && address < 0x8000){
        prg_ram[address & 0x1FFF] = value;
    }else if(address >= 0x8000){
        //bankswitch?
        switch(mapper){
            case 2:{
                int val;
                if(header[5] <= 8) val = value & 7;
                else if(header[5] <= 16) val = value & 15;
                for(int i=0;i<4;i++) prg_rom[i] = prg_banks + val * 0x4000 + 0x1000 * i;
                break;
            }
            case 3:{
                int val;
                if(header[5] <= 4) val = value & 3;
                else if(header[5] <= 8) val = value & 7;
                else val = value & 0xF;
                for(int i=0;i<8;i++) chr_rom[i] = chr_banks + val * 0x2000 + 0x400 * i;
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
                        if(value + 1 > 2*header[4]){
                            value %= (2*header[4]); //Super Contra?
                        }
                        for(int i=0;i<2;i++){
                            prg_rom[nb + i] = prg_banks + 0x2000 * value + 0x1000 * i;
                        }
                        
                    }
                    
                }   
                if(address % 2 == 0 && address >= 0xA000 && address <= 0xBFFF){
                    if(value & 1) bus->setPPUHorizontal(true);
                    else bus->setPPUHorizontal(false);
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
        I = false;
    }
    
    
}