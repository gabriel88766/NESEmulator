#include "6502.h"
#include <stdio.h>

/* 
Function
int test(int a, int b){
    return a + b;
}
*/

Instruction instructions[256] = {
    {6, &CPU::print7}, {8, &CPU::print10}
};

void CPU::nextInstruction(){
    invoke(instructions[0].instruction, *this);
    printf("%d\n", instructions[0].cycles);
    invoke(instructions[1].instruction, *this);
    printf("%d\n", instructions[1].cycles);
}

void invoke(void (CPU::*function)(), CPU &obj) {
    (obj.*function)();
}

void CPU::print7(){
    printf("7\n");
}

void CPU::print10(){
    printf("10\n");
}
