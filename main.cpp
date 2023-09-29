#include "includes/6502.h"
#include "includes/bus.h"
#include <iostream>

int main(){
    Bus bus;
    CPU cpu;
    Cartridge cartridge;
    PPU ppu;
    bus.connectCPU(&cpu);
    bus.connectCartridge(&cartridge);
    bus.connectPPU(&ppu);
    cartridge.read("testROM/Super_Mario_Bros.nes");
    cpu.powerON();
    //ppu.testMake(); 
    while(true){
        cpu.nextInstruction();
        //bus.readAddress(0x2007);
    }   
}