#include "bus.h"
#include <fstream>
#include <cstring>

void Bus::connectCPU(CPU *cpu){
    this->cpu = cpu;
    this->cpu->connectBus(this); 
}

void Bus::connectCartridge(Cartridge *cartridge){
    this->cartridge = cartridge;
    this->cartridge->connectBus(this);
}

void Bus::connectPPU(PPU *ppu){
    this->ppu = ppu;
    this->ppu->connectBus(this);
}

unsigned char Bus::readAddress(unsigned short address){
    if(address < 0x2000){
        return memory[address & 0x7FF];
    }else if(address < 0x4000){
        return ppu->readMemory(address);
    }else if(address < 0x4020){
        //apu
    }else{
        //cartridge area
        if(address >= 0x6000){
            return cartridge->readMemory(address);
        }
    }
    return 0;
}

void Bus::writeAddress(unsigned short address, unsigned char value){
    if(address < 0x2000){
        memory[address & 0x7FF] = value;
    }else if(address < 0x4000){
        ppu->writeMemory(address, value);
    }else if(address < 0x4020){

    }else{

    }
}

void Bus::setNMI(){
    cpu->setnmi();
}

long long Bus::getCycles(){
    return cpu->total_cycles;
}

void Bus::dumpStack(){
    for(int i=0x100;i<=0x1FF;i++){
        printf("0x%02X ", memory[i]);
    }
    printf("\n");
    fflush(stdout);
}