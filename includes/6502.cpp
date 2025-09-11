#include "6502.h"
#include <cstdio>
#include <cstdlib>

Instruction instructions[256] = {
    {7, &CPU::BRK, &CPU::impl}, //0x00
    {6, &CPU::ORA, &CPU::indX}, //0x01
    {2, &CPU::Illegal, &CPU::impl}, //0x02
    {2, &CPU::Illegal, &CPU::impl}, //0x03
    {2, &CPU::Illegal, &CPU::impl}, //0x04
    {3, &CPU::ORA, &CPU::zpg}, //0x05
    {5, &CPU::ASL, &CPU::zpg}, //0x06
    {2, &CPU::Illegal, &CPU::impl}, //0x07
    {3, &CPU::PHP, &CPU::impl}, //0x08
    {2, &CPU::ORA, &CPU::imm}, //0x09
    {2, &CPU::ASL, &CPU::acc}, //0x0A
    {2, &CPU::Illegal, &CPU::impl}, //0x0B
    {2, &CPU::Illegal, &CPU::impl}, //0x0C
    {4, &CPU::ORA, &CPU::abs}, //0x0D
    {6, &CPU::ASL, &CPU::abs}, //0x0E
    {2, &CPU::Illegal, &CPU::impl}, //0x0F
    {2, &CPU::BPL, &CPU::rel}, //0x10
    {5, &CPU::ORA, &CPU::indY}, //0x11
    {2, &CPU::Illegal, &CPU::impl}, //0x12
    {2, &CPU::Illegal, &CPU::impl}, //0x13
    {2, &CPU::Illegal, &CPU::impl}, //0x14
    {4, &CPU::ORA, &CPU::zpgX}, //0x15
    {6, &CPU::ASL, &CPU::zpgX}, //0x16
    {2, &CPU::Illegal, &CPU::impl}, //0x17
    {2, &CPU::CLC, &CPU::impl}, //0x18
    {4, &CPU::ORA, &CPU::absY}, //0x19
    {2, &CPU::Illegal, &CPU::impl}, //0x1A
    {2, &CPU::Illegal, &CPU::impl}, //0x1B
    {2, &CPU::Illegal, &CPU::impl}, //0x1C
    {4, &CPU::ORA, &CPU::absX}, //0x1D
    {7, &CPU::ASL, &CPU::absX}, //0x1E
    {2, &CPU::Illegal, &CPU::impl}, //0x1F
    {6, &CPU::JSR, &CPU::abs}, //0x20
    {6, &CPU::AND, &CPU::indX}, //0x21
    {2, &CPU::Illegal, &CPU::impl}, //0x22
    {2, &CPU::Illegal, &CPU::impl}, //0x23
    {3, &CPU::BIT, &CPU::zpg}, //0x24
    {3, &CPU::AND, &CPU::zpg}, //0x25
    {5, &CPU::ROL, &CPU::zpg}, //0x26
    {2, &CPU::Illegal, &CPU::impl}, //0x27
    {4, &CPU::PLP, &CPU::impl}, //0x28
    {2, &CPU::AND, &CPU::imm}, //0x29
    {2, &CPU::ROL, &CPU::acc}, //0x2A
    {2, &CPU::Illegal, &CPU::impl}, //0x2B
    {4, &CPU::BIT, &CPU::abs}, //0x2C
    {4, &CPU::AND, &CPU::abs}, //0x2D
    {6, &CPU::ROL, &CPU::abs}, //0x2E
    {2, &CPU::Illegal, &CPU::impl}, //0x2F
    {2, &CPU::BMI, &CPU::rel}, //0x30
    {5, &CPU::AND, &CPU::indY}, //0x31
    {2, &CPU::Illegal, &CPU::impl}, //0x32
    {2, &CPU::Illegal, &CPU::impl}, //0x33
    {2, &CPU::Illegal, &CPU::impl}, //0x34
    {4, &CPU::AND, &CPU::zpgX}, //0x35
    {6, &CPU::ROL, &CPU::zpgX}, //0x36
    {2, &CPU::Illegal, &CPU::impl}, //0x37
    {2, &CPU::SEC, &CPU::impl}, //0x38
    {4, &CPU::AND, &CPU::absY}, //0x39
    {2, &CPU::Illegal, &CPU::impl}, //0x3A
    {2, &CPU::Illegal, &CPU::impl}, //0x3B
    {2, &CPU::Illegal, &CPU::impl}, //0x3C
    {4, &CPU::AND, &CPU::absX}, //0x3D
    {7, &CPU::ROL, &CPU::absX}, //0x3E
    {2, &CPU::Illegal, &CPU::impl}, //0x3F
    {6, &CPU::RTI, &CPU::impl}, //0x40
    {6, &CPU::EOR, &CPU::indX}, //0x41
    {2, &CPU::Illegal, &CPU::impl}, //0x42
    {2, &CPU::Illegal, &CPU::impl}, //0x43
    {2, &CPU::Illegal, &CPU::impl}, //0x44
    {3, &CPU::EOR, &CPU::zpg}, //0x45
    {5, &CPU::LSR, &CPU::zpg}, //0x46
    {2, &CPU::Illegal, &CPU::impl}, //0x47
    {3, &CPU::PHA, &CPU::impl}, //0x48
    {2, &CPU::EOR, &CPU::imm}, //0x49
    {2, &CPU::LSR, &CPU::acc}, //0x4A
    {2, &CPU::Illegal, &CPU::impl}, //0x4B
    {3, &CPU::JMP, &CPU::abs}, //0x4C
    {4, &CPU::EOR, &CPU::abs}, //0x4D
    {6, &CPU::LSR, &CPU::abs}, //0x4E
    {2, &CPU::Illegal, &CPU::impl}, //0x4F
    {2, &CPU::BVC, &CPU::rel}, //0x50
    {5, &CPU::EOR, &CPU::indY}, //0x51
    {2, &CPU::Illegal, &CPU::impl}, //0x52
    {2, &CPU::Illegal, &CPU::impl}, //0x53
    {2, &CPU::Illegal, &CPU::impl}, //0x54
    {4, &CPU::EOR, &CPU::zpgX}, //0x55
    {6, &CPU::LSR, &CPU::zpgX}, //0x56
    {2, &CPU::Illegal, &CPU::impl}, //0x57
    {2, &CPU::CLI, &CPU::impl}, //0x58
    {4, &CPU::EOR, &CPU::absY}, //0x59
    {2, &CPU::Illegal, &CPU::impl}, //0x5A
    {2, &CPU::Illegal, &CPU::impl}, //0x5B
    {2, &CPU::Illegal, &CPU::impl}, //0x5C
    {4, &CPU::EOR, &CPU::absX}, //0x5D
    {7, &CPU::LSR, &CPU::absX}, //0x5E
    {2, &CPU::Illegal, &CPU::impl}, //0x5F
    {6, &CPU::RTS, &CPU::impl}, //0x60
    {6, &CPU::ADC, &CPU::indX}, //0x61
    {2, &CPU::Illegal, &CPU::impl}, //0x62
    {2, &CPU::Illegal, &CPU::impl}, //0x63
    {2, &CPU::Illegal, &CPU::impl}, //0x64
    {3, &CPU::ADC, &CPU::zpg}, //0x65
    {5, &CPU::ROR, &CPU::zpg}, //0x66
    {2, &CPU::Illegal, &CPU::impl}, //0x67
    {4, &CPU::PLA, &CPU::impl}, //0x68
    {2, &CPU::ADC, &CPU::imm}, //0x69
    {2, &CPU::ROR, &CPU::acc}, //0x6A
    {2, &CPU::Illegal, &CPU::impl}, //0x6B
    {5, &CPU::JMP, &CPU::ind}, //0x6C
    {4, &CPU::ADC, &CPU::abs}, //0x6D
    {6, &CPU::ROR, &CPU::abs}, //0x6E
    {2, &CPU::Illegal, &CPU::impl}, //0x6F
    {2, &CPU::BVS, &CPU::rel}, //0x70
    {5, &CPU::ADC, &CPU::indY}, //0x71
    {2, &CPU::Illegal, &CPU::impl}, //0x72
    {2, &CPU::Illegal, &CPU::impl}, //0x73
    {2, &CPU::Illegal, &CPU::impl}, //0x74
    {4, &CPU::ADC, &CPU::zpgX}, //0x75
    {6, &CPU::ROR, &CPU::zpgX}, //0x76
    {2, &CPU::Illegal, &CPU::impl}, //0x77
    {2, &CPU::SEI, &CPU::impl}, //0x78
    {4, &CPU::ADC, &CPU::absY}, //0x79
    {2, &CPU::Illegal, &CPU::impl}, //0x7A
    {2, &CPU::Illegal, &CPU::impl}, //0x7B
    {2, &CPU::Illegal, &CPU::impl}, //0x7C
    {4, &CPU::ADC, &CPU::absX}, //0x7D
    {7, &CPU::ROR, &CPU::absX}, //0x7E
    {2, &CPU::Illegal, &CPU::impl}, //0x7F
    {2, &CPU::Illegal, &CPU::impl}, //0x80
    {6, &CPU::STA, &CPU::indX}, //0x81
    {2, &CPU::Illegal, &CPU::impl}, //0x82
    {2, &CPU::Illegal, &CPU::impl}, //0x83
    {3, &CPU::STY, &CPU::zpg}, //0x84
    {3, &CPU::STA, &CPU::zpg}, //0x85
    {3, &CPU::STX, &CPU::zpg}, //0x86
    {2, &CPU::Illegal, &CPU::impl}, //0x87
    {2, &CPU::DEY, &CPU::impl}, //0x88
    {2, &CPU::Illegal, &CPU::impl}, //0x89
    {2, &CPU::TXA, &CPU::impl}, //0x8A
    {2, &CPU::Illegal, &CPU::impl}, //0x8B
    {4, &CPU::STY, &CPU::abs}, //0x8C
    {4, &CPU::STA, &CPU::abs}, //0x8D
    {4, &CPU::STX, &CPU::abs}, //0x8E
    {2, &CPU::Illegal, &CPU::impl}, //0x8F
    {2, &CPU::BCC, &CPU::rel}, //0x90
    {6, &CPU::STA, &CPU::indY}, //0x91
    {2, &CPU::Illegal, &CPU::impl}, //0x92
    {2, &CPU::Illegal, &CPU::impl}, //0x93
    {4, &CPU::STY, &CPU::zpgX}, //0x94
    {4, &CPU::STA, &CPU::zpgX}, //0x95
    {4, &CPU::STX, &CPU::zpgY}, //0x96
    {2, &CPU::Illegal, &CPU::impl}, //0x97
    {2, &CPU::TYA, &CPU::impl}, //0x98
    {5, &CPU::STA, &CPU::absY}, //0x99
    {2, &CPU::TXS, &CPU::impl}, //0x9A
    {2, &CPU::Illegal, &CPU::impl}, //0x9B
    {2, &CPU::Illegal, &CPU::impl}, //0x9C
    {5, &CPU::STA, &CPU::absX}, //0x9D
    {2, &CPU::Illegal, &CPU::impl}, //0x9E
    {2, &CPU::Illegal, &CPU::impl}, //0x9F
    {2, &CPU::LDY, &CPU::imm}, //0xA0
    {6, &CPU::LDA, &CPU::indX}, //0xA1
    {2, &CPU::LDX, &CPU::imm}, //0xA2
    {2, &CPU::Illegal, &CPU::impl}, //0xA3
    {3, &CPU::LDY, &CPU::zpg}, //0xA4
    {3, &CPU::LDA, &CPU::zpg}, //0xA5
    {3, &CPU::LDX, &CPU::zpg}, //0xA6
    {2, &CPU::Illegal, &CPU::impl}, //0xA7
    {2, &CPU::TAY, &CPU::impl}, //0xA8
    {2, &CPU::LDA, &CPU::imm}, //0xA9
    {2, &CPU::TAX, &CPU::impl}, //0xAA
    {2, &CPU::Illegal, &CPU::impl}, //0xAB
    {4, &CPU::LDY, &CPU::abs}, //0xAC
    {4, &CPU::LDA, &CPU::abs}, //0xAD
    {4, &CPU::LDX, &CPU::abs}, //0xAE
    {2, &CPU::Illegal, &CPU::impl}, //0xAF
    {2, &CPU::BCS, &CPU::rel}, //0xB0
    {5, &CPU::LDA, &CPU::indY}, //0xB1
    {2, &CPU::Illegal, &CPU::impl}, //0xB2
    {2, &CPU::Illegal, &CPU::impl}, //0xB3
    {4, &CPU::LDY, &CPU::zpgX}, //0xB4
    {4, &CPU::LDA, &CPU::zpgX}, //0xB5
    {4, &CPU::LDX, &CPU::zpgY}, //0xB6
    {2, &CPU::Illegal, &CPU::impl}, //0xB7
    {2, &CPU::CLV, &CPU::impl}, //0xB8
    {4, &CPU::LDA, &CPU::absY}, //0xB9
    {2, &CPU::TSX, &CPU::impl}, //0xBA
    {2, &CPU::Illegal, &CPU::impl}, //0xBB
    {4, &CPU::LDY, &CPU::absX}, //0xBC
    {4, &CPU::LDA, &CPU::absX}, //0xBD
    {4, &CPU::LDX, &CPU::absY}, //0xBE
    {2, &CPU::Illegal, &CPU::impl}, //0xBF
    {2, &CPU::CPY, &CPU::imm}, //0xC0
    {6, &CPU::CMP, &CPU::indX}, //0xC1
    {2, &CPU::Illegal, &CPU::impl}, //0xC2
    {2, &CPU::Illegal, &CPU::impl}, //0xC3
    {3, &CPU::CPY, &CPU::zpg}, //0xC4
    {3, &CPU::CMP, &CPU::zpg}, //0xC5
    {5, &CPU::DEC, &CPU::zpg}, //0xC6
    {2, &CPU::Illegal, &CPU::impl}, //0xC7
    {2, &CPU::INY, &CPU::impl}, //0xC8
    {2, &CPU::CMP, &CPU::imm}, //0xC9
    {2, &CPU::DEX, &CPU::impl}, //0xCA
    {2, &CPU::Illegal, &CPU::impl}, //0xCB
    {4, &CPU::CPY, &CPU::abs}, //0xCC
    {4, &CPU::CMP, &CPU::abs}, //0xCD
    {6, &CPU::DEC, &CPU::abs}, //0xCE
    {2, &CPU::Illegal, &CPU::impl}, //0xCF
    {2, &CPU::BNE, &CPU::rel}, //0xD0
    {5, &CPU::CMP, &CPU::indY}, //0xD1
    {2, &CPU::Illegal, &CPU::impl}, //0xD2
    {2, &CPU::Illegal, &CPU::impl}, //0xD3
    {2, &CPU::Illegal, &CPU::impl}, //0xD4
    {4, &CPU::CMP, &CPU::zpgX}, //0xD5
    {6, &CPU::DEC, &CPU::zpgX}, //0xD6
    {2, &CPU::Illegal, &CPU::impl}, //0xD7
    {2, &CPU::CLD, &CPU::impl}, //0xD8
    {4, &CPU::CMP, &CPU::absY}, //0xD9
    {2, &CPU::Illegal, &CPU::impl}, //0xDA
    {2, &CPU::Illegal, &CPU::impl}, //0xDB
    {2, &CPU::Illegal, &CPU::impl}, //0xDC
    {4, &CPU::CMP, &CPU::absX}, //0xDD
    {7, &CPU::DEC, &CPU::absX}, //0xDE
    {2, &CPU::Illegal, &CPU::impl}, //0xDF
    {2, &CPU::CPX, &CPU::imm}, //0xE0
    {6, &CPU::SBC, &CPU::indX}, //0xE1
    {2, &CPU::Illegal, &CPU::impl}, //0xE2
    {2, &CPU::Illegal, &CPU::impl}, //0xE3
    {3, &CPU::CPX, &CPU::zpg}, //0xE4
    {3, &CPU::SBC, &CPU::zpg}, //0xE5
    {5, &CPU::INC, &CPU::zpg}, //0xE6
    {2, &CPU::Illegal, &CPU::impl}, //0xE7
    {2, &CPU::INX, &CPU::impl}, //0xE8
    {2, &CPU::SBC, &CPU::imm}, //0xE9
    {2, &CPU::NOP, &CPU::impl}, //0xEA
    {2, &CPU::Illegal, &CPU::impl}, //0xEB
    {4, &CPU::CPX, &CPU::abs}, //0xEC
    {4, &CPU::SBC, &CPU::abs}, //0xED
    {6, &CPU::INC, &CPU::abs}, //0xEE
    {2, &CPU::Illegal, &CPU::impl}, //0xEF
    {2, &CPU::BEQ, &CPU::rel}, //0xF0
    {5, &CPU::SBC, &CPU::indY}, //0xF1
    {2, &CPU::Illegal, &CPU::impl}, //0xF2
    {2, &CPU::Illegal, &CPU::impl}, //0xF3
    {2, &CPU::Illegal, &CPU::impl}, //0xF4
    {4, &CPU::SBC, &CPU::zpgX}, //0xF5
    {6, &CPU::INC, &CPU::zpgX}, //0xF6
    {2, &CPU::Illegal, &CPU::impl}, //0xF7
    {2, &CPU::SED, &CPU::impl}, //0xF8
    {4, &CPU::SBC, &CPU::absY}, //0xF9
    {2, &CPU::Illegal, &CPU::impl}, //0xFA
    {2, &CPU::Illegal, &CPU::impl}, //0xFB
    {2, &CPU::Illegal, &CPU::impl}, //0xFC
    {4, &CPU::SBC, &CPU::absX}, //0xFD
    {7, &CPU::INC, &CPU::absX}, //0xFE
    {2, &CPU::Illegal, &CPU::impl}  //0xFF 
};

//Instructions 265-618
void CPU::ADC(){
    unsigned char Oper2 = bus->readAddress(memory);
    Addition(Oper2);
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
        isAccumulator = false;
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
    unsigned char oper = bus->readAddress(memory);
    unsigned char result = A & oper;
    checkN(oper);
    oper & 0x40 ? setV() : clearV();
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
    setB();
    Push(P);
    setI();
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
    clearI();
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
        isAccumulator = false;
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
    //setB();
    Push(P);
}
void CPU::PLA(){
    A = Pull();
    checkN(A);
    checkZ(A);
}
void CPU::PLP(){
    P = (Pull() & 0xCF) | (P & 0x30);
}
void CPU::ROL(){
    unsigned char valueShifted, result;
    if(isAccumulator){
        valueShifted = A;
        result = valueShifted << 1;
        if(getC()) result |= 0x01;
        A = result;
        isAccumulator = false;
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
        isAccumulator = false;
    }else{
        valueShifted = bus->readAddress(memory);
        result = valueShifted >> 1;
        if(getC()) result |= 0x80;
        bus->writeAddress(memory, result);
    }
    valueShifted & 0x01 ? setC() : clearC();
    checkZ(result);
    checkN(result);
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
    unsigned char Oper2 = ~bus->readAddress(memory);
    Addition(Oper2);
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
void CPU::Addition(unsigned short Oper2){
    if(getD()){
        //Decimal mode is not used in NES, may have bugs.
        //Probably run slowly than binary.

        //SBC -- reverts binary case and convert to decimal case
        if(instructions[opcode].instruction == &CPU::SBC){
            Oper2 = ~Oper2; //revert
            Oper2 =  (0x90 - (Oper2 & 0xF0)) + (0x9 - (Oper2 & 0xF));
        }
        unsigned short Oper1 = A;
        unsigned char low = (Oper1 & 0xF) + (Oper2 & 0xF) + (getC() ? 1 : 0);
        unsigned short result = 0;
        if(low > 9){
            low += 6;
        }
        result += (Oper1 & 0xF0) + (Oper2 & 0xF0) + low;
        if(result > 0x99){
            setC();
            result -= 0xA0;
        }else{
            clearC();
        } 
        checkZ(result);
        A = result;        
    }else{
        unsigned short Oper1 = A;
        unsigned short result = Oper1 + Oper2 + (getC() ? 1 : 0);
        A = (unsigned char)result;
        A == 0 ? setZ() : clearZ();
        A & 0x80 ? setN() : clearN();
        result > 0xFF ? setC() : clearC();
        //test overflow 1
        (Oper1 ^ result ) & (Oper2 ^ result) & 0x80 ? setV() : clearV();
    }
    
}
void CPU::Branch(){
    unsigned short oldPC = PC;
    PC = memory;
    if((oldPC ^ PC) & 0xFF00) total_cycles++;
    total_cycles++;
}
void CPU::Compare(unsigned char reg){
    unsigned char memValue = bus->readAddress(memory);
    memValue <= reg ? setC() : clearC();
    reg -= memValue;
    checkZ(reg);
    checkN(reg);
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
    // exit(1);
}

//Address Mode 620-700
void CPU::acc(){
    isAccumulator = true;
}
void CPU::abs(){
    unsigned short low = bus->readAddress(PC++);
    unsigned short high = bus->readAddress(PC++);
    memory = low | (high << 8);
}
void CPU::absX(){
    unsigned short low = bus->readAddress(PC++);
    unsigned short high = bus->readAddress(PC++);
    memory = (high << 8) | low;
    if(((memory + X) & 0xFF00) != (memory & 0xFF00)){ //page cross
        if((opcode != 0x9D) && ((opcode & 0x0F) != 0x0E)){ 
            total_cycles++;
        }
    } 
    memory += X;
}
void CPU::absY(){
    unsigned short low = bus->readAddress(PC++);
    unsigned short high = bus->readAddress(PC++);
    memory = (high << 8) | low;
    if(((memory + Y) & 0xFF00) != (memory & 0xFF00)){ //page cross
        if(opcode != 0x99){ 
            total_cycles++;
        }
    } 
    memory += Y;
}
void CPU::imm(){
    memory = PC++;
}
void CPU::impl(){
    //Nothing to do.
}
void CPU::ind(){
    unsigned short low = bus->readAddress(PC++);
    unsigned short high = bus->readAddress(PC++);
    unsigned short memoryIndirect = (high << 8) | low;
    low = bus->readAddress(memoryIndirect);
    memoryIndirect = (memoryIndirect & 0xFF00) + ((memoryIndirect + 1) & 0xFF); //no page cross
    high = bus->readAddress(memoryIndirect);
    memory = (high << 8) | low;
}
void CPU::indX(){
    unsigned char memIndirect = bus->readAddress(PC++);
    memIndirect += X; //must be zero page, < 0xFF
    unsigned short low = bus->readAddress(memIndirect++);
    unsigned short high = bus->readAddress(memIndirect);
    memory = (high << 8) | low;
}
void CPU::indY(){
    unsigned char memIndirect = bus->readAddress(PC++);
    unsigned short low = bus->readAddress(memIndirect++); //memIndirect must be < 0xFF
    unsigned short high = bus->readAddress(memIndirect);
    memory = (high << 8) | low;
    if(((memory + Y) & 0xFF00) != (memory & 0xFF00)){ //page cross
        if(opcode != 0x91 ){
            total_cycles++;
        }
    } 
    memory += Y;
}
void CPU::rel(){
    char value = bus->readAddress(PC++);
    memory = PC + value;
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

//Flags 702-772
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

//Controllers and debug
void CPU::printState(unsigned short opc){
    printf( "A : 0x%02X, X : 0x%02X, Y : 0x%02X, P : 0x%02X, S : 0x%02X PC : 0x%04X, opcode = 0x%02X\n", 
        A, X, Y, P, S, opc, opcode); //registers and opcode.
        fflush(stdout);
}

void CPU::nextInstruction(){
    if(irq_pin){
        if(!getI()){
            irq_pin = false;//interrupt handled
            irq();
        }
    }
    if(nmi_pin){
        nmi_pin = false;
        nmi();
    }
    unsigned short opc = PC;
    opcode = bus->readAddress(PC++);
    invoke(instructions[opcode].address_mode, *this);
    invoke(instructions[opcode].instruction, *this);
    total_cycles += instructions[opcode].cycles;
    // printState(opc);
}

void CPU::powerON(){
    P = 0x34;
    A = X = Y = 0;
    S = 0xFD;
    PC = 0x8000;
    total_cycles = 0;
}

void CPU::reset(){
    setI();
    S -= 3;
    P |= 0x04;
    PC = bus->readAddress(0xFFFD) << 8;
    PC |= bus->readAddress(0xFFFC);
}

void CPU::irq(){
    unsigned char low = PC;
    unsigned char high = PC >> 8;
    Push(high);
    Push(low); 
    clearB();
    Push(P);
    setI();
    PC = bus->readAddress(0xFFFF) << 8;
    PC |= bus->readAddress(0xFFFE);
    total_cycles += 7;
}

void CPU::nmi(){
    unsigned char low = PC;
    unsigned char high = PC >> 8;
    Push(high);
    Push(low); 
    clearB();
    Push(P);
    setI();
    PC = bus->readAddress(0xFFFB) << 8;
    PC |= bus->readAddress(0xFFFA);
    total_cycles += 7;
}

void CPU::connectBus(Bus *bus){
    this->bus = bus;
}

void invoke(void (CPU::*function)(), CPU &obj) {
    (obj.*function)();
}

void CPU::setirq(){
    irq_pin = true;
}

void CPU::setnmi(){
    nmi_pin = true;
}
