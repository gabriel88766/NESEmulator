/**/

#ifndef CPU6502_H
#define CPU6502_H

#include <functional>

/*
Function
int test(int a, int b);
*/
class CPU{
private:
    unsigned char A, X, Y, S;
    unsigned short PC;
public:
    void print7();
    void print10();

    void nextInstruction();
};

struct Instruction{
    int cycles;
    void(CPU::*instruction)();
};

extern Instruction instructions[];



void invoke(void (CPU::*function)(), CPU &obj);

#endif 