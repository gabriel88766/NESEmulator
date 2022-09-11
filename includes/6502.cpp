#include "6502.h"
#include <cstdio>
#include <cstdlib>

Instruction instructions[256] = {
    {0, &CPU::Illegal, NULL}, //0x00
    {0, &CPU::Illegal, NULL}, //0x01
    {0, &CPU::Illegal, NULL}, //0x02
    {0, &CPU::Illegal, NULL}, //0x03
    {0, &CPU::Illegal, NULL}, //0x04
    {0, &CPU::Illegal, NULL}, //0x05
    {0, &CPU::Illegal, NULL}, //0x06
    {0, &CPU::Illegal, NULL}, //0x07
    {0, &CPU::Illegal, NULL}, //0x08
    {0, &CPU::Illegal, NULL}, //0x09
    {0, &CPU::Illegal, NULL}, //0x0A
    {0, &CPU::Illegal, NULL}, //0x0B
    {0, &CPU::Illegal, NULL}, //0x0C
    {0, &CPU::Illegal, NULL}, //0x0D
    {0, &CPU::Illegal, NULL}, //0x0E
    {0, &CPU::Illegal, NULL}, //0x0F
    {0, &CPU::BPL, &CPU::rel}, //0x10
    {0, &CPU::Illegal, NULL}, //0x11
    {0, &CPU::Illegal, NULL}, //0x12
    {0, &CPU::Illegal, NULL}, //0x13
    {0, &CPU::Illegal, NULL}, //0x14
    {0, &CPU::Illegal, NULL}, //0x15
    {0, &CPU::Illegal, NULL}, //0x16
    {0, &CPU::Illegal, NULL}, //0x17
    {0, &CPU::Illegal, NULL}, //0x18
    {0, &CPU::Illegal, NULL}, //0x19
    {0, &CPU::Illegal, NULL}, //0x1A
    {0, &CPU::Illegal, NULL}, //0x1B
    {0, &CPU::Illegal, NULL}, //0x1C
    {0, &CPU::Illegal, NULL}, //0x1D
    {0, &CPU::Illegal, NULL}, //0x1E
    {0, &CPU::Illegal, NULL}, //0x1F
    {0, &CPU::Illegal, NULL}, //0x20
    {0, &CPU::Illegal, NULL}, //0x21
    {0, &CPU::Illegal, NULL}, //0x22
    {0, &CPU::Illegal, NULL}, //0x23
    {0, &CPU::Illegal, NULL}, //0x24
    {0, &CPU::Illegal, NULL}, //0x25
    {0, &CPU::Illegal, NULL}, //0x26
    {0, &CPU::Illegal, NULL}, //0x27
    {0, &CPU::Illegal, NULL}, //0x28
    {0, &CPU::Illegal, NULL}, //0x29
    {0, &CPU::Illegal, NULL}, //0x2A
    {0, &CPU::Illegal, NULL}, //0x2B
    {0, &CPU::Illegal, NULL}, //0x2C
    {0, &CPU::Illegal, NULL}, //0x2D
    {0, &CPU::Illegal, NULL}, //0x2E
    {0, &CPU::Illegal, NULL}, //0x2F
    {0, &CPU::BMI, &CPU::rel}, //0x30
    {0, &CPU::Illegal, NULL}, //0x31
    {0, &CPU::Illegal, NULL}, //0x32
    {0, &CPU::Illegal, NULL}, //0x33
    {0, &CPU::Illegal, NULL}, //0x34
    {0, &CPU::Illegal, NULL}, //0x35
    {0, &CPU::Illegal, NULL}, //0x36
    {0, &CPU::Illegal, NULL}, //0x37
    {0, &CPU::Illegal, NULL}, //0x38
    {0, &CPU::Illegal, NULL}, //0x39
    {0, &CPU::Illegal, NULL}, //0x3A
    {0, &CPU::Illegal, NULL}, //0x3B
    {0, &CPU::Illegal, NULL}, //0x3C
    {0, &CPU::Illegal, NULL}, //0x3D
    {0, &CPU::Illegal, NULL}, //0x3E
    {0, &CPU::Illegal, NULL}, //0x3F
    {0, &CPU::Illegal, NULL}, //0x40
    {0, &CPU::Illegal, NULL}, //0x41
    {0, &CPU::Illegal, NULL}, //0x42
    {0, &CPU::Illegal, NULL}, //0x43
    {0, &CPU::Illegal, NULL}, //0x44
    {0, &CPU::Illegal, NULL}, //0x45
    {0, &CPU::Illegal, NULL}, //0x46
    {0, &CPU::Illegal, NULL}, //0x47
    {0, &CPU::Illegal, NULL}, //0x48
    {0, &CPU::Illegal, NULL}, //0x49
    {0, &CPU::Illegal, NULL}, //0x4A
    {0, &CPU::Illegal, NULL}, //0x4B
    {0, &CPU::Illegal, NULL}, //0x4C
    {0, &CPU::Illegal, NULL}, //0x4D
    {0, &CPU::Illegal, NULL}, //0x4E
    {0, &CPU::Illegal, NULL}, //0x4F
    {0, &CPU::BVC, &CPU::rel}, //0x50
    {0, &CPU::Illegal, NULL}, //0x51
    {0, &CPU::Illegal, NULL}, //0x52
    {0, &CPU::Illegal, NULL}, //0x53
    {0, &CPU::Illegal, NULL}, //0x54
    {0, &CPU::Illegal, NULL}, //0x55
    {0, &CPU::Illegal, NULL}, //0x56
    {0, &CPU::Illegal, NULL}, //0x57
    {0, &CPU::Illegal, NULL}, //0x58
    {0, &CPU::Illegal, NULL}, //0x59
    {0, &CPU::Illegal, NULL}, //0x5A
    {0, &CPU::Illegal, NULL}, //0x5B
    {0, &CPU::Illegal, NULL}, //0x5C
    {0, &CPU::Illegal, NULL}, //0x5D
    {0, &CPU::Illegal, NULL}, //0x5E
    {0, &CPU::Illegal, NULL}, //0x5F
    {0, &CPU::Illegal, NULL}, //0x60
    {0, &CPU::Illegal, NULL}, //0x61
    {0, &CPU::Illegal, NULL}, //0x62
    {0, &CPU::Illegal, NULL}, //0x63
    {0, &CPU::Illegal, NULL}, //0x64
    {0, &CPU::Illegal, NULL}, //0x65
    {0, &CPU::Illegal, NULL}, //0x66
    {0, &CPU::Illegal, NULL}, //0x67
    {0, &CPU::Illegal, NULL}, //0x68
    {0, &CPU::Illegal, NULL}, //0x69
    {0, &CPU::Illegal, NULL}, //0x6A
    {0, &CPU::Illegal, NULL}, //0x6B
    {0, &CPU::Illegal, NULL}, //0x6C
    {0, &CPU::Illegal, NULL}, //0x6D
    {0, &CPU::Illegal, NULL}, //0x6E
    {0, &CPU::Illegal, NULL}, //0x6F
    {0, &CPU::BVS, &CPU::rel}, //0x70
    {0, &CPU::Illegal, NULL}, //0x71
    {0, &CPU::Illegal, NULL}, //0x72
    {0, &CPU::Illegal, NULL}, //0x73
    {0, &CPU::Illegal, NULL}, //0x74
    {0, &CPU::Illegal, NULL}, //0x75
    {0, &CPU::Illegal, NULL}, //0x76
    {0, &CPU::Illegal, NULL}, //0x77
    {0, &CPU::Illegal, NULL}, //0x78
    {0, &CPU::Illegal, NULL}, //0x79
    {0, &CPU::Illegal, NULL}, //0x7A
    {0, &CPU::Illegal, NULL}, //0x7B
    {0, &CPU::Illegal, NULL}, //0x7C
    {0, &CPU::Illegal, NULL}, //0x7D
    {0, &CPU::Illegal, NULL}, //0x7E
    {0, &CPU::Illegal, NULL}, //0x7F
    {0, &CPU::Illegal, NULL}, //0x80
    {0, &CPU::Illegal, NULL}, //0x81
    {0, &CPU::Illegal, NULL}, //0x82
    {0, &CPU::Illegal, NULL}, //0x83
    {0, &CPU::Illegal, NULL}, //0x84
    {0, &CPU::Illegal, NULL}, //0x85
    {0, &CPU::Illegal, NULL}, //0x86
    {0, &CPU::Illegal, NULL}, //0x87
    {0, &CPU::Illegal, NULL}, //0x88
    {0, &CPU::Illegal, NULL}, //0x89
    {0, &CPU::Illegal, NULL}, //0x8A
    {0, &CPU::Illegal, NULL}, //0x8B
    {0, &CPU::Illegal, NULL}, //0x8C
    {0, &CPU::Illegal, NULL}, //0x8D
    {0, &CPU::Illegal, NULL}, //0x8E
    {0, &CPU::Illegal, NULL}, //0x8F
    {0, &CPU::BCC, &CPU::rel}, //0x90
    {0, &CPU::Illegal, NULL}, //0x91
    {0, &CPU::Illegal, NULL}, //0x92
    {0, &CPU::Illegal, NULL}, //0x93
    {0, &CPU::Illegal, NULL}, //0x94
    {0, &CPU::Illegal, NULL}, //0x95
    {0, &CPU::Illegal, NULL}, //0x96
    {0, &CPU::Illegal, NULL}, //0x97
    {0, &CPU::Illegal, NULL}, //0x98
    {0, &CPU::Illegal, NULL}, //0x99
    {0, &CPU::Illegal, NULL}, //0x9A
    {0, &CPU::Illegal, NULL}, //0x9B
    {0, &CPU::Illegal, NULL}, //0x9C
    {0, &CPU::Illegal, NULL}, //0x9D
    {0, &CPU::Illegal, NULL}, //0x9E
    {0, &CPU::Illegal, NULL}, //0x9F
    {0, &CPU::Illegal, NULL}, //0xA0
    {0, &CPU::Illegal, NULL}, //0xA1
    {0, &CPU::Illegal, NULL}, //0xA2
    {0, &CPU::Illegal, NULL}, //0xA3
    {0, &CPU::Illegal, NULL}, //0xA4
    {0, &CPU::Illegal, NULL}, //0xA5
    {0, &CPU::Illegal, NULL}, //0xA6
    {0, &CPU::Illegal, NULL}, //0xA7
    {0, &CPU::Illegal, NULL}, //0xA8
    {0, &CPU::Illegal, NULL}, //0xA9
    {0, &CPU::Illegal, NULL}, //0xAA
    {0, &CPU::Illegal, NULL}, //0xAB
    {0, &CPU::Illegal, NULL}, //0xAC
    {0, &CPU::Illegal, NULL}, //0xAD
    {0, &CPU::Illegal, NULL}, //0xAE
    {0, &CPU::Illegal, NULL}, //0xAF
    {0, &CPU::BCS, &CPU::rel}, //0xB0
    {0, &CPU::Illegal, NULL}, //0xB1
    {0, &CPU::Illegal, NULL}, //0xB2
    {0, &CPU::Illegal, NULL}, //0xB3
    {0, &CPU::Illegal, NULL}, //0xB4
    {0, &CPU::Illegal, NULL}, //0xB5
    {0, &CPU::Illegal, NULL}, //0xB6
    {0, &CPU::Illegal, NULL}, //0xB7
    {0, &CPU::Illegal, NULL}, //0xB8
    {0, &CPU::Illegal, NULL}, //0xB9
    {0, &CPU::Illegal, NULL}, //0xBA
    {0, &CPU::Illegal, NULL}, //0xBB
    {0, &CPU::Illegal, NULL}, //0xBC
    {0, &CPU::Illegal, NULL}, //0xBD
    {0, &CPU::Illegal, NULL}, //0xBE
    {0, &CPU::Illegal, NULL}, //0xBF
    {0, &CPU::Illegal, NULL}, //0xC0
    {0, &CPU::Illegal, NULL}, //0xC1
    {0, &CPU::Illegal, NULL}, //0xC2
    {0, &CPU::Illegal, NULL}, //0xC3
    {0, &CPU::Illegal, NULL}, //0xC4
    {0, &CPU::Illegal, NULL}, //0xC5
    {0, &CPU::Illegal, NULL}, //0xC6
    {0, &CPU::Illegal, NULL}, //0xC7
    {0, &CPU::Illegal, NULL}, //0xC8
    {0, &CPU::Illegal, NULL}, //0xC9
    {0, &CPU::Illegal, NULL}, //0xCA
    {0, &CPU::Illegal, NULL}, //0xCB
    {0, &CPU::Illegal, NULL}, //0xCC
    {0, &CPU::Illegal, NULL}, //0xCD
    {0, &CPU::Illegal, NULL}, //0xCE
    {0, &CPU::Illegal, NULL}, //0xCF
    {0, &CPU::BNE, &CPU::rel}, //0xD0
    {0, &CPU::Illegal, NULL}, //0xD1
    {0, &CPU::Illegal, NULL}, //0xD2
    {0, &CPU::Illegal, NULL}, //0xD3
    {0, &CPU::Illegal, NULL}, //0xD4
    {0, &CPU::Illegal, NULL}, //0xD5
    {0, &CPU::Illegal, NULL}, //0xD6
    {0, &CPU::Illegal, NULL}, //0xD7
    {0, &CPU::Illegal, NULL}, //0xD8
    {0, &CPU::Illegal, NULL}, //0xD9
    {0, &CPU::Illegal, NULL}, //0xDA
    {0, &CPU::Illegal, NULL}, //0xDB
    {0, &CPU::Illegal, NULL}, //0xDC
    {0, &CPU::Illegal, NULL}, //0xDD
    {0, &CPU::Illegal, NULL}, //0xDE
    {0, &CPU::Illegal, NULL}, //0xDF
    {0, &CPU::Illegal, NULL}, //0xE0
    {0, &CPU::Illegal, NULL}, //0xE1
    {0, &CPU::Illegal, NULL}, //0xE2
    {0, &CPU::Illegal, NULL}, //0xE3
    {0, &CPU::Illegal, NULL}, //0xE4
    {0, &CPU::Illegal, NULL}, //0xE5
    {0, &CPU::Illegal, NULL}, //0xE6
    {0, &CPU::Illegal, NULL}, //0xE7
    {0, &CPU::Illegal, NULL}, //0xE8
    {0, &CPU::Illegal, NULL}, //0xE9
    {0, &CPU::Illegal, NULL}, //0xEA
    {0, &CPU::Illegal, NULL}, //0xEB
    {0, &CPU::Illegal, NULL}, //0xEC
    {0, &CPU::Illegal, NULL}, //0xED
    {0, &CPU::Illegal, NULL}, //0xEE
    {0, &CPU::Illegal, NULL}, //0xEF
    {0, &CPU::BEQ, &CPU::rel}, //0xF0
    {0, &CPU::Illegal, NULL}, //0xF1
    {0, &CPU::Illegal, NULL}, //0xF2
    {0, &CPU::Illegal, NULL}, //0xF3
    {0, &CPU::Illegal, NULL}, //0xF4
    {0, &CPU::Illegal, NULL}, //0xF5
    {0, &CPU::Illegal, NULL}, //0xF6
    {0, &CPU::Illegal, NULL}, //0xF7
    {0, &CPU::Illegal, NULL}, //0xF8
    {0, &CPU::Illegal, NULL}, //0xF9
    {0, &CPU::Illegal, NULL}, //0xFA
    {0, &CPU::Illegal, NULL}, //0xFB
    {0, &CPU::Illegal, NULL}, //0xFC
    {0, &CPU::Illegal, NULL}, //0xFD
    {0, &CPU::Illegal, NULL}, //0xFE
    {0, &CPU::Illegal, NULL}  //0xFF 
};
CPU::CPU(){
    this->powerON();
}
//Instructions
void CPU::ADC(){
    if(getD()){
        //Not implemented (decimal mode)
    }else{
        unsigned char Oper1 = A;
        unsigned short Oper2 = bus->readAddress(memory) + getC() ? 1 : 0;
        unsigned short result = A + Oper2;
        A = (unsigned char)result;
        A == 0 ? setZ() : clearZ();
        A & 0x80 ? setN() : clearN();
        result > 0xFF ? setC() : clearC();
        (Oper1 ^ result ) & (Oper2 ^ result) & 0x80 ? setV() : clearV();
    }
}
void CPU::AND(){
    A = A & bus->readAddress(memory);
    checkZ(A);
    checkN(A);
}
void CPU::ASL(){
    unsigned char valueShifted, result;
    if(isAccumulator){
        valueShifted = A;
        result = valueShifted << 1;
        A = result;
    }else{
        valueShifted = bus->readAddress(memory);
        result = valueShifted << 1;
        bus->writeAddress(memory, result);
    }
    valueShifted & 0x80 ? setC() : clearC();
    checkZ(result);
    checkN(result);
}
void CPU::BCC(){
    if(!getC()) Branch();
}
void CPU::BCS(){
    if(getC()) Branch();
}
void CPU::BEQ(){
    if(getZ()) Branch();
}
void CPU::BIT(){
    unsigned char result = A ^ bus->readAddress(memory);
    checkN(result);
    result & 0x40 ? setV() : clearV();
    checkZ(result); 
}
void CPU::BMI(){
    if(getN()) Branch();
}
void CPU::BNE(){
    if(!getZ()) Branch();
}
void CPU::BPL(){
    if(!getN()) Branch();
}
void CPU::BRK(){
    PC++;
    unsigned char low = PC;
    unsigned char high = PC >> 8;
    Push(high);
    Push(low); 
    Push(P);
    PC = bus->readAddress(0xFFFF) << 8;
    PC |= bus->readAddress(0xFFFE);
}
void CPU::BVC(){
    if(!getV()) Branch();
}
void CPU::BVS(){
    if(getV()) Branch();
}
void CPU::CLC(){
    clearC();
}
void CPU::CLD(){
    clearD();
}
void CPU::CLI(){
    clearD();
}
void CPU::CLV(){
    clearV();
}
void CPU::CMP(){
    Compare(A);
}
void CPU::CPX(){
    Compare(X);
}
void CPU::CPY(){
    Compare(Y);
}
void CPU::DEC(){
    unsigned char value = bus->readAddress(memory);
    value--;
    bus->writeAddress(memory, value);
    checkZ(value);
    checkN(value);
}
void CPU::DEX(){
    X--;
    checkZ(X);
    checkN(X);
}
void CPU::DEY(){
    Y--;
    checkZ(Y);
    checkN(Y);
}
void CPU::EOR(){
    A ^= bus->readAddress(memory);
    checkZ(A);
    checkN(A);
}
void CPU::INC(){
    unsigned char value = bus->readAddress(memory);
    value++;
    bus->writeAddress(memory, value);
    checkZ(value);
    checkN(value);
}
void CPU::INX(){
    X++;
    checkZ(X);
    checkN(X);
}
void CPU::INY(){
    Y++;
    checkZ(Y);
    checkN(Y);
}
void CPU::JMP(){
    PC = memory;
}
void CPU::JSR(){
    unsigned short retAddress = PC - 1;
    unsigned char low = retAddress;
    unsigned char high = retAddress >> 8;
    Push(high);
    Push(low);
    PC = memory;
}
void CPU::LDA(){
    A = bus->readAddress(memory);
    checkZ(A);
    checkN(A);
}
void CPU::LDX(){
    X = bus->readAddress(memory);
    checkZ(X);
    checkN(X);
}
void CPU::LDY(){
    Y = bus->readAddress(memory);
    checkZ(Y);
    checkN(Y);
}
void CPU::LSR(){
    unsigned char valueShifted, result;
    if(isAccumulator){
        valueShifted = A;
        result = valueShifted >> 1;
        A = result;
    }else{
        valueShifted = bus->readAddress(memory);
        result = valueShifted >> 1;
        bus->writeAddress(memory, result);
    }
    valueShifted & 0x01 ? setC() : clearC();
    checkZ(result);
    clearN();
}
void CPU::NOP(){
    //Nothing
}
void CPU::ORA(){
    A = A | bus->readAddress(memory);
    checkZ(A);
    checkN(A);
}
void CPU::PHA(){
    Push(A);
}
void CPU::PHP(){
    Push(P);
}
void CPU::PLA(){
    A = Pull();
}
void CPU::PLP(){
    P = Pull();
}
void CPU::ROL(){
    unsigned char valueShifted, result;
    if(isAccumulator){
        valueShifted = A;
        result = valueShifted << 1;
        if(getC()) result |= 0x01;
        A = result;
    }else{
        valueShifted = bus->readAddress(memory);
        result = valueShifted << 1;
        if(getC()) result |= 0x01;
        bus->writeAddress(memory, result);
    }
    valueShifted & 0x80 ? setC() : clearC();
    checkZ(result);
    checkN(result);
}
void CPU::ROR(){
    unsigned char valueShifted, result;
    if(isAccumulator){
        valueShifted = A;
        result = valueShifted >> 1;
        if(getC()) result |= 0x80;
        A = result;
    }else{
        valueShifted = bus->readAddress(memory);
        result = valueShifted >> 1;
        if(getC()) result |= 0x80;
        bus->writeAddress(memory, result);
    }
    valueShifted & 0x01 ? setC() : clearC();
    checkZ(result);
    clearN();
}
void CPU::RTI(){
    P = Pull();
    PC = Pull();
    unsigned short high = Pull();
    PC |= high << 8;
}
void CPU::RTS(){
    PC = Pull();
    unsigned short high = Pull();
    PC |= high << 8;
    PC++;
}
void CPU::SBC(){

}
void CPU::SEC(){
    setC();
}
void CPU::SED(){
    setD();
}
void CPU::SEI(){
    setI();
}
void CPU::STA(){
    bus->writeAddress(memory, A);
}
void CPU::STX(){
    bus->writeAddress(memory, X);
}
void CPU::STY(){
    bus->writeAddress(memory, Y);
}
void CPU::TAX(){
    X = A;
    checkZ(X);
    checkN(X);
}
void CPU::TAY(){
    Y = A;
    checkZ(Y);
    checkN(Y);
}
void CPU::TSX(){
    X = S;
    checkZ(X);
    checkN(X);
}
void CPU::TXA(){
    A = X;
    checkZ(A);
    checkN(A);
}
void CPU::TXS(){
    S = X;
}
void CPU::TYA(){
    A = Y;
    checkZ(A);
    checkN(A);
}
void CPU::Branch(){
    PC = memory;
    total_cycles++;
}
void CPU::Compare(unsigned char reg){
    unsigned char memValue = bus->readAddress(memory);
    memValue <= reg ? setC() : clearC();
    reg -= memValue;
    checkZ(memValue);
    checkN(memValue);
}
unsigned char CPU::Pull(){
    S++;
    return bus->readAddress(0x100 + S);
}
void CPU::Push(unsigned char value){
    bus->writeAddress(0x100 + S, value);
    S--;
}
void CPU::Illegal(){
    printf("Reached an Illegal OPCODE that is not defined.\n");
    exit(1);
}
//Address Mode
void CPU::acc(){
    isAccumulator = true;
}
void CPU::abs(){
    unsigned short low = bus->readAddress(PC++);
    unsigned short high = bus->readAddress(PC++);
    memory = low | (high << 8);
}
void absX(){

}
void absY();
void CPU::imm(){
    memory = PC++;
}
void CPU::impl(){

}
void CPU::ind(){

}
void CPU::indX(){

}
void CPU::indY(){

}
void CPU::rel(){
    unsigned char value = bus->readAddress(PC++);
    unsigned short oldPC = PC;
    memory = PC + (char)value;
    if((oldPC ^ PC) & 0xFF00) total_cycles++;
}
void CPU::zpg(){
    memory = bus->readAddress(PC++);
}
void CPU::zpgX(){
    memory = bus->readAddress(PC++);
    memory += X;
    memory &= 0xFF;
}
void CPU::zpgY(){
    memory = bus->readAddress(PC++);
    memory += Y;
    memory &= 0xFF;    
}

//Flags
void CPU::setC(){
    P |= 0x01;
}
void CPU::clearC(){
    P &= 0xFE;
}
bool CPU::getC(){
    return P & 0x01;
}
void CPU::setZ(){
    P |= 0x02;
}
void CPU::clearZ(){
    P &= 0xFD;
}
bool CPU::getZ(){
    return P & 0x02;
}
void CPU::setI(){
    P |= 0x04;
}
void CPU::clearI(){
    P &= 0xFB;
}
bool CPU::getI(){
    return P & 0x04;
}
void CPU::setD(){
    P |= 0x08;
}
void CPU::clearD(){
    P &= 0xF7;
}
bool CPU::getD(){
    return P & 0x08;
}
void CPU::setB(){
    P |= 0x10;
}
void CPU::clearB(){
    P &= 0xEF;
}
bool CPU::getB(){
    return P & 0x10;
}
void CPU::setV(){
    P |= 0x40;
}
void CPU::clearV(){
    P &= 0xBF;
}
bool CPU::getV(){
    return P & 0x40;
}
void CPU::setN(){
    P |= 0x80;
}
void CPU::clearN(){
    P &= 0x7F;
}
bool CPU::getN(){
    return P & 0x80;
}

void CPU::checkZ(unsigned char value){
    value == 0x00 ? setZ() : clearZ();
}
void CPU::checkN(unsigned char value){
    value & 0x80 ? setN() : clearN();
}


//Others and debug
void CPU::printState(){
    printf( "A : 0x%02X, X : 0x%02X, Y : 0x%02X, S : 0x%02X, PC : 0x%04X\n",
        A, X, Y, S, PC ); //registers.
}

void CPU::nextInstruction(){
    opcode = bus->readAddress(PC++);
    invoke(instructions[opcode].address_mode, *this);
    invoke(instructions[opcode].instruction, *this);
    total_cycles += instructions[opcode].cycles;
}

void CPU::powerON(){

}

void CPU::reset(){

}

void CPU::connectBus(Bus *bus){
    this->bus = bus;
}


void invoke(void (CPU::*function)(), CPU &obj) {
    (obj.*function)();
}