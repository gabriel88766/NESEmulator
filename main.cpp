#include "includes/6502.h"
#include "includes/bus.h"
#include <iostream>

int main(){
    Bus bus;
    CPU cpu;
    
    bus.connectCPU(&cpu);
    bus.readTest();
    while(true){
        cpu.nextInstruction();
    }
}