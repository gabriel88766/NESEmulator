#ifndef CPU6502_H
#define CPU6502_H

class CPU{
private:
    unsigned char A, X, Y, S;
    unsigned short PC;
public:
    void print7();
    void print10();

    void printState();
    void nextInstruction();
};

struct Instruction{
    int cycles;
    void(CPU::*instruction)();
};

extern Instruction instructions[];

void invoke(void (CPU::*function)(), CPU &obj);


#endif 