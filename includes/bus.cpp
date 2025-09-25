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
    unsigned char ret;
    if(address < 0x2000){
        ret = cpu->readAddress(address & 0x7FF); //readmemory[address & 0x7FF];
    }else if(address < 0x4000){
        ret = ppu->readMemory(address);
    }else if(address < 0x4020){
        //apu
        if(address == 0x4016){
            ret = (regbut1 & 1) ? 0 : 1;  // active low
            if (!strobe) {
                regbut1 >>= 1;
            }
        }else if(address == 0x4017){
            ret = (regbut2 & 1) ? 0 : 1;  // active low
            if (!strobe) {
                regbut2 >>= 1;
            }
        }else if(address == 0x4015) return apu->readMemory(0x15);
        else return ret = last_value;
    }else if(address < 0x6000){
        ret = last_value;
    }else{
        //cartridge area
        if(address >= 0x6000){
            ret = cartridge->readMemory(address);
        }
    }
    if(open){
        ret &= ~open;
        ret |= open & last_value;
        open = 0;
    }
    return last_value = ret;
}

void Bus::writeAddress(unsigned short address, unsigned char value){
    last_value = value;
    if(address < 0x2000){
        cpu->writeAddress(address & 0x7FF, value);
    }else if(address < 0x4000){
        // if(address > 0x2007) ppu->writeMemory(address, value);
        ppu->writeMemory(address, value);
    }else if(address < 0x4020){
        if(address == 0x4014){
            
            unsigned short begin = value;
            begin <<= 8;
            for(unsigned short j = 0; j < 0x100; j++){
                ppu->writeOAM(cpu->readAddress(begin + j));
                cpu->newCycle();
                cpu->newCycle();
            }
            cpu->newCycle();
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

long long Bus::getCycles(){
    return cpu->total_cycles;
}

void Bus::setPPUHorizontal(bool value){
    ppu->horizontal = value;
}


//this won't handle any error
unsigned char Bus::readCartridge(unsigned short address){
    return cartridge->readMemory(address);
}

void Bus::writeCartridge(unsigned short address, unsigned char value){
    cartridge->writeMemory(address, value);
}