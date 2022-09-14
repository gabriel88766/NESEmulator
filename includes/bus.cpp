#include "bus.h"
#include <fstream>
#include <cstring>


void Bus::connectCPU(CPU *cpu){
    this->cpu = cpu;
    this->cpu->connectBus(this); 
}

unsigned char Bus::readAddress(unsigned short address){
    return memory[address]; //test
    if(address < 0x2000){
        return memory[address & 0x800];
    }
    return 0;
}

void Bus::writeAddress(unsigned short address, unsigned char value){
    memory[address] = value; //test line1
    return; //test line2
    if(address < 0x2000){
        memory[address & 0x800] = value;
    }
}

void Bus::readTest(){
    std::ifstream input("testROM/6502_functional_test.bin", std::ios::binary);
    input.read( (char*)( &memory[0] ), 0x10000 );
}