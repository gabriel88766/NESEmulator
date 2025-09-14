//This class will be used to mappers.
#include "cartridge.h"
#include <fstream>

Cartridge::~Cartridge(){
    delete[] header;
    delete[] prg_banks;
    delete[] chr_banks;
    delete[] chr_rom;
    delete[] prg_rom;
}

void Cartridge::connectBus(Bus *bus){
    this->bus = bus;
}

void Cartridge::read(const char *filename){
    std::ifstream input(filename, std::ios::binary);
    header = new unsigned char[16];
    input.read( (char*)header, 16 );
    if(header[0] = 'N' && header[1] == 'E' && header[2] == 'S' && header[3] == 0x1A){
        //TODO: assert iNes, this emulator isn't indented to support NES 2.0
        mapper = (header[7] & 0xF0) | (header[6] >> 4);
        if(header[6] & 1){
            bus->setPPUHorizontal(false);
        }else{
            bus->setPPUHorizontal(true);
        }
        if(header[6] & 2) ram = true;
        prg_banks = new unsigned char[0x4000 * header[4]];
        chr_banks = new unsigned char[0x2000 * header[5]];
        prg_rom = new unsigned char[0x8000];
        chr_rom = new unsigned char[0x2000];
        input.read((char *)prg_banks, 0x4000 * header[4]);
        input.read((char *)chr_banks, 0x2000 * header[5]);


        switch(mapper){
            case 0:
                if(header[4] == 2){
                    for(int j=0;j<0x8000;j++){
                        prg_rom[j] = prg_banks[j];
                    }
                }else if(header[4] == 1){
                    for(int j=0;j<0x4000;j++){
                        prg_rom[j] = prg_rom[j+0x4000] = prg_banks[j];
                    }
                }
                if(header[5] == 1){
                    for(int j=0;j<0x2000;j++){
                        chr_rom[j] = chr_banks[j];
                    }
                }else{
                    ram = true;
                    for(int j=0;j<0x2000;j++) chr_rom[j] = 0;
                }
            case 3:
                if(header[4] == 2){
                    for(int j=0;j<0x8000;j++){
                        prg_rom[j] = prg_banks[j];
                    }
                }
                //bank 00 default
                for(int j=0;j<0x2000;j++){
                    chr_rom[j] = chr_banks[j];
                }
        }
        
        
        
        

        
    }else{
        //return false.
    }


    
}

unsigned char Cartridge::readMemory(unsigned short address){
    if(address >= 0x8000){
        return prg_rom[address & 0x7FFF];
    }
    if(address < 0x8000){
        return chr_rom[address & 0x1FFF];
    }
    return 0;
}
void Cartridge::writeMemory(unsigned short address, unsigned char value){
    if(ram && address >= 0x6000 && address < 0x8000){
        chr_rom[address & 0x1FFF] = value;
    }
    if(address >= 0x8000){
        //bankswitch?
        switch(mapper){
            //TODO
            case 3:
                int val;
                if(header[5] <= 4) val = value & 3;
                else if(header[5] <= 8) val = value & 7;
                else val = value & 0xF;
                printf("%d\n", val);
                for(int j=0;j<0x2000;j++){
                    chr_rom[j] = chr_banks[j + val*0x2000];
                }
        }
    }
}