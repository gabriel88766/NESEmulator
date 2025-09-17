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
        return last_value = memory[address & 0x7FF];
    }else if(address < 0x4000){
        return last_value = ppu->readMemory(address);
    }else if(address < 0x4020){
        //apu
        if(address == 0x4016){
            unsigned char result = (regbut1 & 1) ? 0 : 1;  // active low
            if (!strobe) {
                regbut1 >>= 1;
            }
            return result;
        }else if(address == 0x4017){
            unsigned char result = (regbut2 & 1) ? 0 : 1;  // active low
            if (!strobe) {
                regbut2 >>= 1;
            }
            return result;
        }else return apu->readMemory(address & 0x1F);
    }else if(address < 0x6000){
        return last_value;
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
            
            unsigned short begin = value;
            begin <<= 8;
            for(unsigned short j = 0; j < 0x100; j++){
                ppu->writeOAM(memory[begin + j]);
                cpu->total_cycles += 2;
                for(int u=0;u<6;u++) movePPU();
            }
            cpu->total_cycles += 1;
            for(int u=0;u<3;u++) movePPU();
        }else if(address == 0x4016){
            strobe = value & 1;
            if (strobe) {
                regbut1 = button1;
                regbut2 = button2;
            }
        }else apu->writeMemory(address & 0x1F, value);
    }else if(address >= 0x6000){
        // printf("here\n");
        cartridge->writeMemory(address, value);
    }
}

void Bus::setIRQ(bool value){
    cpu->setirq(value);
}

void Bus::setNMI(){
    cpu->setnmi();
}

long long Bus::getCycles(){
    return cpu->total_cycles;
}

void Bus::setPPUHorizontal(bool value){
    ppu->horizontal = value;
}

void Bus::movePPU(){
    ppu->move();
}

void Bus::loadCHR(unsigned char *data, int beg, int end){
    for(int j=beg;j<end;j++){
        ppu->VRAM[j] = data[j-beg];
    }
}

void Bus::setRAMPPU(){
    ppu->setRAM();
}

void Bus::clockAPU(){
    apu->clock();
}

unsigned char Bus::readCartridge(unsigned short address){
    return 0;
}

void Bus::writeCartridge(unsigned short address, unsigned char value){

}