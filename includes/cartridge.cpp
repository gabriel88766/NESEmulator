#include "cartridge.h"
#include <fstream>

Cartridge::~Cartridge(){
    delete[] header;
    delete[] prg_rom;
    delete[] chr_rom;
}

void Cartridge::connectBus(Bus *bus){
    this->bus = bus;
}

void Cartridge::read(const char *filename){// default nes 40K
    std::ifstream input(filename, std::ios::binary);
    header = new unsigned char[16];
    input.read( (char*)header, 16 );
    prg_rom = new unsigned char[0x8000];
    input.read((char*)prg_rom, 0x8000);
    chr_rom = new unsigned char[0x4000];
    input.read((char *)chr_rom, 0x4000);
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
    
}