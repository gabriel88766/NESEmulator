#include "includes/6502.h"
#include "includes/bus.h"
#include <cstdio>

long long int nvb = 0;
const long long int clock_frame = 29829;
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
    ppu.testMake(); 
    cpu.printState();
    while(true){
        if(cpu.total_cycles >= nvb){
            nvb += clock_frame;
            ppu.vblank();
        }
        cpu.nextInstruction();
        //bus.readAddress(0x2007);
    }   
}