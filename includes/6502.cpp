#include "6502.h"
#include <stdio.h>

Instruction instructions[256] = {
    {6, &CPU::print7}, {8, &CPU::print10}
};

void CPU::print7(){
    A = 0xA3;
}

void CPU::print10(){
    printf("10\n");
}

void CPU::printState(){
    printf( "A : 0x%02X, X : 0x%02X, Y : 0x%02X, S : 0x%02X, PC : 0x%04X\n",
        A, X, Y, S, PC ); //registers.
}

void CPU::nextInstruction(){
    // Next instruc -> invoke(instructions[opcode].instruction, *this);
    // totalcycles += instructions[opcode].cycles);
    invoke(instructions[0].instruction, *this);
}

void invoke(void (CPU::*function)(), CPU &obj) {
    (obj.*function)();
}