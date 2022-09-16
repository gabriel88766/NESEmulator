#ifndef CPU6502_H
#define CPU6502_H

#include "bus.h"

class Bus;

class CPU{
private:
    unsigned char A, X, Y, S, P;
    unsigned short PC;
    unsigned short memory;
    unsigned char opcode;
    long long int total_cycles;
    int extra_cycles;
    bool isAccumulator = false; //memory vs accumulator instruction
    Bus *bus;
    bool irq_pin = false;
    bool nmi_pin = false;
public:
    CPU(){
        this->powerON();
    }
    //Instructions
    void ADC();
    void AND();
    void ASL();
    void BCC();
    void BCS();
    void BEQ();
    void BIT();
    void BMI();
    void BNE();
    void BPL();
    void BRK();
    void BVC();
    void BVS();
    void CLC();
    void CLD();
    void CLI();
    void CLV();
    void CMP();
    void CPX();
    void CPY();
    void DEC();
    void DEX();
    void DEY();
    void EOR();
    void INC();
    void INX();
    void INY();
    void JMP();
    void JSR();
    void LDA();
    void LDX();
    void LDY();
    void LSR();
    void NOP();
    void ORA();
    void PHA();
    void PHP();
    void PLA();
    void PLP();
    void ROL();
    void ROR();
    void RTI();
    void RTS();
    void SBC();
    void SEC();
    void SED();
    void SEI();
    void STA();
    void STX();
    void STY();
    void TAX();
    void TAY();
    void TSX();
    void TXA();
    void TXS();
    void TYA();
    void Addition(unsigned short Oper2); //ADC and SBC
    void Branch();
    void Compare(unsigned char reg);
    unsigned char Pull();
    void Push(unsigned char value);
    void Illegal();
    //Address mode
    void acc();
    void abs();
    void absX();
    void absY();
    void imm();
    void impl();
    void ind();
    void indX();
    void indY();
    void rel();
    void zpg();
    void zpgX();
    void zpgY();
    //Flags
    void setC();
    void clearC();
    bool getC();
    void setZ();
    void clearZ();
    bool getZ();
    void setI();
    void clearI();
    bool getI();
    void setD();
    void clearD();
    bool getD();
    void setB();
    void clearB();
    bool getB();
    void setV();
    void clearV();
    bool getV();
    void setN();
    void clearN();
    bool getN();
    void checkZ(unsigned char value);
    void checkN(unsigned char value);
    //Controllers and debug
    void printState();
    void nextInstruction();
    void powerON();
    void reset();
    void irq();
    void nmi();
    void connectBus(Bus *bus);
};

struct Instruction{
    int cycles;
    void(CPU::*instruction)();
    void(CPU::*address_mode)();
};

void invoke(void (CPU::*function)(), CPU &obj);


#endif 