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

void Bus::connectAPU(APU *apu){
    this->apu = apu;
    this->apu->connectBus(this);
}

unsigned char Bus::readAddress(unsigned short address){
    if(address < 0x2000){
        return memory[address & 0x7FF];
    }else if(address < 0x4000){
        return ppu->readMemory(address);
    }else if(address < 0x4020){
        //apu
        if(address == 0x4016){
            unsigned char result = (regbut & 1) ? 0 : 1;  // active low
            if (!strobe) {
                regbut >>= 1;
            }
            return result;
        }else apu->readMemory(address & 0x1F);
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
        if(address == 0x4014){
            //special
            cpu->total_cycles += 513;
            unsigned short begin = value;
            begin <<= 8;
            for(unsigned short j = 0; j < 0x100; j++){
                ppu->writeOAM(memory[begin + j]);
                // ppu->writeOAM(j, memory[begin + j]);
            }
        }else if(address == 0x4016){
            strobe = value & 1;
            if (strobe) {
                regbut = buttons;
            }
            
        }else apu->writeMemory(address & 0x1F, value);
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