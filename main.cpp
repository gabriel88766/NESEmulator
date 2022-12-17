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
    cartridge.read("testROM/Super_mario_brothers.nes");
    cpu.powerON();
    ppu.testMake(); 
    cpu.printState();
    /*while(true){
        cpu.nextInstruction();
        //bus.readAddress(0x2007);
    }*/
   
}