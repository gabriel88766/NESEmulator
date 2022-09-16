#include "bus.h"
#include <fstream>
#include <cstring>


void Bus::connectCPU(CPU *cpu){
    this->cpu = cpu;
    this->cpu->connectBus(this); 
}

unsigned char Bus::readAddress(unsigned short address){
    if(address < 0x2000){
        return memory[address & 0x800];
    }
    return 0;
}

void Bus::writeAddress(unsigned short address, unsigned char value){
    if(address < 0x2000){
        memory[address & 0x800] = value;
    }
}
