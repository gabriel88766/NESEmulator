//This class will be used to mappers.
//DEFAULT 8KB VRAM from 0x6000 to 0x8000
#include "cartridge.h"
#include <fstream>

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
        // if(header[6] & 2) ram = true;
        prg_banks = new unsigned char[0x4000 * header[4]];
        chr_banks = new unsigned char[0x2000 * header[5]];
        prg_rom = new unsigned char[0x8000];
        prg_ram = new unsigned char[0x2000];
        chr_rom = new unsigned char[0x2000];
        input.read((char *)prg_banks, 0x4000 * header[4]);
        input.read((char *)chr_banks, 0x2000 * header[5]);


        switch(mapper){
            case 0:
                if(header[4] == 2){
                    load(prg_rom, prg_banks, 0x8000);
                }else if(header[4] == 1){
                    load(prg_rom, prg_banks, 0x4000);
                    load(prg_rom + 0x4000, prg_banks, 0x4000);
                }
                if(header[5] == 1){
                    load(chr_rom, chr_banks, 0x2000);
                }else ram = true;
                break;
            case 2:
                load(prg_rom, prg_banks, 0x4000);
                load(prg_rom + 0x4000, prg_banks + (header[4] - 1) * 0x4000, 0x4000);
                ram = true;
                break;
            case 3:
                if(header[4] == 2){
                    load(prg_rom, prg_banks, 0x8000);
                }else if(header[4] == 1){
                    load(prg_rom, prg_banks, 0x4000);
                    load(prg_rom + 0x4000, prg_banks, 0x4000);
                }
                load(chr_rom, chr_banks, 0x2000);//bank 00 default
                break;
            case 185:
                if(header[4] == 2){
                    load(prg_rom, prg_banks, 0x8000);
                }else if(header[4] == 1){
                    load(prg_rom, prg_banks, 0x4000);
                    load(prg_rom + 0x4000, prg_banks, 0x4000);
                }
                load(chr_rom, chr_banks, 0x2000);
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
        return chr_rom[address];
    }

    if(address >= 0x8000){
        return prg_rom[address & 0x7FFF];
    }
    if(address >= 0x6000 && address < 0x8000){
        return prg_ram[address & 0x1FFF];
    }
    return 0; //never should happen
}
void Cartridge::writeMemory(unsigned short address, unsigned char value){
    if(address <= 0x1FFF){
        if(ram){
            chr_rom[address] = value;
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
                load(prg_rom, prg_banks + val*0x4000, 0x4000);
                break;
            }
            case 3:{
                int val;
                if(header[5] <= 4) val = value & 3;
                else if(header[5] <= 8) val = value & 7;
                else val = value & 0xF;
                load(chr_rom, chr_banks + val*0x2000, 0x2000);
                
                break;
            }
        }
    }
}