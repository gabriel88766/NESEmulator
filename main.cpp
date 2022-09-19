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
    //ppu.testMake(); 
    while(true){
        //cpu.nextInstruction();
    
    }


    //Make images
    /*
    Image img;
    img.makeImage(256, 256);
    for(int i = 0; i < 256; i++){
        for(int j = 0; j < 256; j++){
            unsigned char value = (i+j)/2;
            img.setPixel(i, j, { value, value, value});
        }
    }
    img.writeImage("images/data.bmp");
    */
}