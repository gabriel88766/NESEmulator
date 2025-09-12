#include "apu.h"
#include <iostream>

APU::APU(){

}

void APU::connectBus(Bus *bus){
    this->bus = bus;
}



unsigned char APU::readMemory(unsigned short address){
    return reg[address];
}   
void APU::writeMemory(unsigned short address, unsigned char value){
    reg[address] = value;
}