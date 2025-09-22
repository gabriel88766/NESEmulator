#include "6502.h"
#include <cstdio>
#include <cstdlib>

Instruction instructions[256] = {
    {7, &CPU::BRK, &CPU::impl}, //0x00
    {6, &CPU::ORA, &CPU::indX}, //0x01
    {2, &CPU::Illegal, &CPU::impl}, //0x02
    {8, &CPU::SLO, &CPU::indX}, //0x03
    {3, &CPU::NOP, &CPU::zpg}, //0x04
    {3, &CPU::ORA, &CPU::zpg}, //0x05
    {5, &CPU::ASL, &CPU::zpg}, //0x06
    {5, &CPU::SLO, &CPU::zpg}, //0x07
    {3, &CPU::PHP, &CPU::impl}, //0x08
    {2, &CPU::ORA, &CPU::imm}, //0x09
    {2, &CPU::ASL, &CPU::acc}, //0x0A
    {2, &CPU::ANC, &CPU::imm}, //0x0B
    {4, &CPU::NOP, &CPU::abs}, //0x0C
    {4, &CPU::ORA, &CPU::abs}, //0x0D
    {6, &CPU::ASL, &CPU::abs}, //0x0E
    {6, &CPU::SLO, &CPU::abs}, //0x0F
    {2, &CPU::BPL, &CPU::rel}, //0x10
    {5, &CPU::ORA, &CPU::indY}, //0x11
    {2, &CPU::Illegal, &CPU::impl}, //0x12
    {8, &CPU::SLO, &CPU::indY}, //0x13
    {4, &CPU::NOP, &CPU::zpgX}, //0x14
    {4, &CPU::ORA, &CPU::zpgX}, //0x15
    {6, &CPU::ASL, &CPU::zpgX}, //0x16
    {6, &CPU::SLO, &CPU::zpgX}, //0x17
    {2, &CPU::CLC, &CPU::impl}, //0x18
    {4, &CPU::ORA, &CPU::absY}, //0x19
    {2, &CPU::NOP, &CPU::impl}, //0x1A
    {7, &CPU::SLO, &CPU::absY}, //0x1B
    {4, &CPU::NOP, &CPU::absX}, //0x1C
    {4, &CPU::ORA, &CPU::absX}, //0x1D
    {7, &CPU::ASL, &CPU::absX}, //0x1E
    {7, &CPU::SLO, &CPU::absX}, //0x1F
    {6, &CPU::JSR, &CPU::abs}, //0x20
    {6, &CPU::AND, &CPU::indX}, //0x21
    {2, &CPU::Illegal, &CPU::impl}, //0x22
    {8, &CPU::RLA, &CPU::indX}, //0x23
    {3, &CPU::BIT, &CPU::zpg}, //0x24
    {3, &CPU::AND, &CPU::zpg}, //0x25
    {5, &CPU::ROL, &CPU::zpg}, //0x26
    {5, &CPU::RLA, &CPU::zpg}, //0x27
    {4, &CPU::PLP, &CPU::impl}, //0x28
    {2, &CPU::AND, &CPU::imm}, //0x29
    {2, &CPU::ROL, &CPU::acc}, //0x2A
    {2, &CPU::ANC, &CPU::imm}, //0x2B
    {4, &CPU::BIT, &CPU::abs}, //0x2C
    {4, &CPU::AND, &CPU::abs}, //0x2D
    {6, &CPU::ROL, &CPU::abs}, //0x2E
    {6, &CPU::RLA, &CPU::abs}, //0x2F
    {2, &CPU::BMI, &CPU::rel}, //0x30
    {5, &CPU::AND, &CPU::indY}, //0x31
    {2, &CPU::Illegal, &CPU::impl}, //0x32
    {8, &CPU::RLA, &CPU::indY}, //0x33
    {4, &CPU::NOP, &CPU::zpgX}, //0x34
    {4, &CPU::AND, &CPU::zpgX}, //0x35
    {6, &CPU::ROL, &CPU::zpgX}, //0x36
    {7, &CPU::RLA, &CPU::zpgX}, //0x37
    {2, &CPU::SEC, &CPU::impl}, //0x38
    {4, &CPU::AND, &CPU::absY}, //0x39
    {2, &CPU::NOP, &CPU::impl}, //0x3A
    {7, &CPU::RLA, &CPU::absY}, //0x3B
    {4, &CPU::NOP, &CPU::absX}, //0x3C
    {4, &CPU::AND, &CPU::absX}, //0x3D
    {7, &CPU::ROL, &CPU::absX}, //0x3E
    {7, &CPU::RLA, &CPU::absX}, //0x3F
    {6, &CPU::RTI, &CPU::impl}, //0x40
    {6, &CPU::EOR, &CPU::indX}, //0x41
    {2, &CPU::Illegal, &CPU::impl}, //0x42
    {8, &CPU::SRE, &CPU::indX}, //0x43
    {3, &CPU::NOP, &CPU::zpg}, //0x44
    {3, &CPU::EOR, &CPU::zpg}, //0x45
    {5, &CPU::LSR, &CPU::zpg}, //0x46
    {5, &CPU::SRE, &CPU::zpg}, //0x47
    {3, &CPU::PHA, &CPU::impl}, //0x48
    {2, &CPU::EOR, &CPU::imm}, //0x49
    {2, &CPU::LSR, &CPU::acc}, //0x4A
    {2, &CPU::ALR, &CPU::imm}, //0x4B
    {3, &CPU::JMP, &CPU::abs}, //0x4C
    {4, &CPU::EOR, &CPU::abs}, //0x4D
    {6, &CPU::LSR, &CPU::abs}, //0x4E
    {6, &CPU::SRE, &CPU::abs}, //0x4F
    {2, &CPU::BVC, &CPU::rel}, //0x50
    {5, &CPU::EOR, &CPU::indY}, //0x51
    {2, &CPU::Illegal, &CPU::impl}, //0x52
    {8, &CPU::SRE, &CPU::indY}, //0x53
    {4, &CPU::NOP, &CPU::zpgX}, //0x54
    {4, &CPU::EOR, &CPU::zpgX}, //0x55
    {6, &CPU::LSR, &CPU::zpgX}, //0x56
    {6, &CPU::SRE, &CPU::zpgX}, //0x57
    {2, &CPU::CLI, &CPU::impl}, //0x58
    {4, &CPU::EOR, &CPU::absY}, //0x59
    {2, &CPU::NOP, &CPU::impl}, //0x5A
    {7, &CPU::SRE, &CPU::absY}, //0x5B
    {4, &CPU::NOP, &CPU::absX}, //0x5C
    {4, &CPU::EOR, &CPU::absX}, //0x5D
    {7, &CPU::LSR, &CPU::absX}, //0x5E
    {7, &CPU::SRE, &CPU::absX}, //0x5F
    {6, &CPU::RTS, &CPU::impl}, //0x60
    {6, &CPU::ADC, &CPU::indX}, //0x61
    {2, &CPU::Illegal, &CPU::impl}, //0x62
    {8, &CPU::RRA, &CPU::indX}, //0x63
    {3, &CPU::NOP, &CPU::zpg}, //0x64
    {3, &CPU::ADC, &CPU::zpg}, //0x65
    {5, &CPU::ROR, &CPU::zpg}, //0x66
    {5, &CPU::RRA, &CPU::zpg}, //0x67
    {4, &CPU::PLA, &CPU::impl}, //0x68
    {2, &CPU::ADC, &CPU::imm}, //0x69
    {2, &CPU::ROR, &CPU::acc}, //0x6A
    {2, &CPU::ARR, &CPU::imm}, //0x6B
    {5, &CPU::JMP, &CPU::ind}, //0x6C
    {4, &CPU::ADC, &CPU::abs}, //0x6D
    {6, &CPU::ROR, &CPU::abs}, //0x6E
    {6, &CPU::RRA, &CPU::abs}, //0x6F
    {2, &CPU::BVS, &CPU::rel}, //0x70
    {5, &CPU::ADC, &CPU::indY}, //0x71
    {2, &CPU::Illegal, &CPU::impl}, //0x72
    {8, &CPU::RRA, &CPU::indY}, //0x73
    {4, &CPU::NOP, &CPU::zpgX}, //0x74
    {4, &CPU::ADC, &CPU::zpgX}, //0x75
    {6, &CPU::ROR, &CPU::zpgX}, //0x76
    {6, &CPU::RRA, &CPU::zpgX}, //0x77
    {2, &CPU::SEI, &CPU::impl}, //0x78
    {4, &CPU::ADC, &CPU::absY}, //0x79
    {2, &CPU::NOP, &CPU::impl}, //0x7A
    {7, &CPU::RRA, &CPU::absY}, //0x7B
    {4, &CPU::NOP, &CPU::absX}, //0x7C
    {4, &CPU::ADC, &CPU::absX}, //0x7D
    {7, &CPU::ROR, &CPU::absX}, //0x7E
    {7, &CPU::RRA, &CPU::absX}, //0x7F
    {2, &CPU::NOP, &CPU::imm}, //0x80
    {6, &CPU::STA, &CPU::indX}, //0x81
    {2, &CPU::NOP, &CPU::imm}, //0x82
    {6, &CPU::SAX, &CPU::indX}, //0x83
    {3, &CPU::STY, &CPU::zpg}, //0x84
    {3, &CPU::STA, &CPU::zpg}, //0x85
    {3, &CPU::STX, &CPU::zpg}, //0x86
    {3, &CPU::SAX, &CPU::zpg}, //0x87
    {2, &CPU::DEY, &CPU::impl}, //0x88
    {2, &CPU::NOP, &CPU::imm}, //0x89
    {2, &CPU::TXA, &CPU::impl}, //0x8A
    {2, &CPU::XAA, &CPU::imm}, //0x8B
    {4, &CPU::STY, &CPU::abs}, //0x8C
    {4, &CPU::STA, &CPU::abs}, //0x8D
    {4, &CPU::STX, &CPU::abs}, //0x8E
    {4, &CPU::SAX, &CPU::abs}, //0x8F
    {2, &CPU::BCC, &CPU::rel}, //0x90
    {6, &CPU::STA, &CPU::indY}, //0x91
    {2, &CPU::Illegal, &CPU::impl}, //0x92
    {6, &CPU::AHX, &CPU::indY}, //0x93
    {4, &CPU::STY, &CPU::zpgX}, //0x94
    {4, &CPU::STA, &CPU::zpgX}, //0x95
    {4, &CPU::STX, &CPU::zpgY}, //0x96
    {4, &CPU::SAX, &CPU::zpgY}, //0x97
    {2, &CPU::TYA, &CPU::impl}, //0x98
    {5, &CPU::STA, &CPU::absY}, //0x99
    {2, &CPU::TXS, &CPU::impl}, //0x9A
    {5, &CPU::TAS, &CPU::absY}, //0x9B
    {5, &CPU::SHY, &CPU::absX}, //0x9C
    {5, &CPU::STA, &CPU::absX}, //0x9D
    {5, &CPU::SHX, &CPU::absY}, //0x9E
    {5, &CPU::AHX, &CPU::absY}, //0x9F
    {2, &CPU::LDY, &CPU::imm}, //0xA0
    {6, &CPU::LDA, &CPU::indX}, //0xA1
    {2, &CPU::LDX, &CPU::imm}, //0xA2
    {6, &CPU::LAX, &CPU::indX}, //0xA3
    {3, &CPU::LDY, &CPU::zpg}, //0xA4
    {3, &CPU::LDA, &CPU::zpg}, //0xA5
    {3, &CPU::LDX, &CPU::zpg}, //0xA6
    {3, &CPU::LAX, &CPU::zpg}, //0xA7
    {2, &CPU::TAY, &CPU::impl}, //0xA8
    {2, &CPU::LDA, &CPU::imm}, //0xA9
    {2, &CPU::TAX, &CPU::impl}, //0xAA
    {2, &CPU::LAX, &CPU::imm}, //0xAB
    {4, &CPU::LDY, &CPU::abs}, //0xAC
    {4, &CPU::LDA, &CPU::abs}, //0xAD
    {4, &CPU::LDX, &CPU::abs}, //0xAE
    {4, &CPU::LAX, &CPU::abs}, //0xAF
    {2, &CPU::BCS, &CPU::rel}, //0xB0
    {5, &CPU::LDA, &CPU::indY}, //0xB1
    {2, &CPU::Illegal, &CPU::impl}, //0xB2
    {5, &CPU::LAX, &CPU::indY}, //0xB3
    {4, &CPU::LDY, &CPU::zpgX}, //0xB4
    {4, &CPU::LDA, &CPU::zpgX}, //0xB5
    {4, &CPU::LDX, &CPU::zpgY}, //0xB6
    {4, &CPU::LAX, &CPU::zpgY}, //0xB7
    {2, &CPU::CLV, &CPU::impl}, //0xB8
    {4, &CPU::LDA, &CPU::absY}, //0xB9
    {2, &CPU::TSX, &CPU::impl}, //0xBA
    {4, &CPU::LAS, &CPU::absY}, //0xBB
    {4, &CPU::LDY, &CPU::absX}, //0xBC
    {4, &CPU::LDA, &CPU::absX}, //0xBD
    {4, &CPU::LDX, &CPU::absY}, //0xBE
    {4, &CPU::LAX, &CPU::absY}, //0xBF
    {2, &CPU::CPY, &CPU::imm}, //0xC0
    {6, &CPU::CMP, &CPU::indX}, //0xC1
    {2, &CPU::NOP, &CPU::imm}, //0xC2
    {8, &CPU::DCP, &CPU::indX}, //0xC3
    {3, &CPU::CPY, &CPU::zpg}, //0xC4
    {3, &CPU::CMP, &CPU::zpg}, //0xC5
    {5, &CPU::DEC, &CPU::zpg}, //0xC6
    {5, &CPU::DCP, &CPU::zpg}, //0xC7
    {2, &CPU::INY, &CPU::impl}, //0xC8
    {2, &CPU::CMP, &CPU::imm}, //0xC9
    {2, &CPU::DEX, &CPU::impl}, //0xCA
    {2, &CPU::AXS, &CPU::imm}, //0xCB
    {4, &CPU::CPY, &CPU::abs}, //0xCC
    {4, &CPU::CMP, &CPU::abs}, //0xCD
    {6, &CPU::DEC, &CPU::abs}, //0xCE
    {6, &CPU::DCP, &CPU::abs}, //0xCF
    {2, &CPU::BNE, &CPU::rel}, //0xD0
    {5, &CPU::CMP, &CPU::indY}, //0xD1
    {2, &CPU::Illegal, &CPU::impl}, //0xD2
    {8, &CPU::DCP, &CPU::indY}, //0xD3
    {4, &CPU::NOP, &CPU::zpgX}, //0xD4
    {4, &CPU::CMP, &CPU::zpgX}, //0xD5
    {6, &CPU::DEC, &CPU::zpgX}, //0xD6
    {6, &CPU::DCP, &CPU::zpgX}, //0xD7
    {2, &CPU::CLD, &CPU::impl}, //0xD8
    {4, &CPU::CMP, &CPU::absY}, //0xD9
    {2, &CPU::NOP, &CPU::impl}, //0xDA
    {7, &CPU::DCP, &CPU::absY}, //0xDB
    {4, &CPU::NOP, &CPU::absX}, //0xDC
    {4, &CPU::CMP, &CPU::absX}, //0xDD
    {7, &CPU::DEC, &CPU::absX}, //0xDE
    {7, &CPU::DCP, &CPU::absX}, //0xDF
    {2, &CPU::CPX, &CPU::imm}, //0xE0
    {6, &CPU::SBC, &CPU::indX}, //0xE1
    {2, &CPU::NOP, &CPU::imm}, //0xE2
    {8, &CPU::ISC, &CPU::indX}, //0xE3
    {3, &CPU::CPX, &CPU::zpg}, //0xE4
    {3, &CPU::SBC, &CPU::zpg}, //0xE5
    {5, &CPU::INC, &CPU::zpg}, //0xE6
    {6, &CPU::ISC, &CPU::zpg}, //0xE7
    {2, &CPU::INX, &CPU::impl}, //0xE8
    {2, &CPU::SBC, &CPU::imm}, //0xE9
    {2, &CPU::NOP, &CPU::impl}, //0xEA
    {2, &CPU::SBC, &CPU::imm}, //0xEB
    {4, &CPU::CPX, &CPU::abs}, //0xEC
    {4, &CPU::SBC, &CPU::abs}, //0xED
    {6, &CPU::INC, &CPU::abs}, //0xEE
    {6, &CPU::ISC, &CPU::abs}, //0xEF
    {2, &CPU::BEQ, &CPU::rel}, //0xF0
    {5, &CPU::SBC, &CPU::indY}, //0xF1
    {2, &CPU::Illegal, &CPU::impl}, //0xF2
    {8, &CPU::ISC, &CPU::indY}, //0xF3
    {4, &CPU::NOP, &CPU::zpgX}, //0xF4
    {4, &CPU::SBC, &CPU::zpgX}, //0xF5
    {6, &CPU::INC, &CPU::zpgX}, //0xF6
    {6, &CPU::ISC, &CPU::zpgX}, //0xF7
    {2, &CPU::SED, &CPU::impl}, //0xF8
    {4, &CPU::SBC, &CPU::absY}, //0xF9
    {2, &CPU::NOP, &CPU::impl}, //0xFA
    {7, &CPU::ISC, &CPU::absY}, //0xFB
    {4, &CPU::NOP, &CPU::absX}, //0xFC
    {4, &CPU::SBC, &CPU::absX}, //0xFD
    {7, &CPU::INC, &CPU::absX}, //0xFE
    {7, &CPU::ISC, &CPU::absX}  //0xFF 
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
    if(nmi_pin){
        nmi_pin = false;
        PC = bus->readAddress(0xFFFB) << 8;
        PC |= bus->readAddress(0xFFFA);
    }else{
        PC = bus->readAddress(0xFFFF) << 8;
        PC |= bus->readAddress(0xFFFE);
    }
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
    Push(P | 0x30);
}
void CPU::PLA(){
    A = Pull();
    checkN(A);
    checkZ(A);
}
void CPU::PLP(){
    unsigned char val = Pull();
    P = (val & 0xCB) | (P & 0x34);
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

//illegal opcodes:
void CPU::AHX(){
    // bus->writeAddress(memory, A & X & H);
}
void CPU::ALR(){
    AND();
    isAccumulator = true;
    LSR();
}
void CPU::ANC(){
    A = A & bus->readAddress(memory);
    if(A & 0x80) setC();
    else clearC();
    checkN(A);
    checkZ(A);
}
void CPU::ARR(){
    AND();
    isAccumulator = true;
    ROR();
}
void CPU::AXS(){
    if((A & X) <= bus->readAddress(memory)) setC();
    else clearC();
    X = (A & X) - bus->readAddress(memory);
    checkN(X);
    checkZ(X);
}
void CPU::DCP(){
    DEC();
    CMP();
}
void CPU::ISC(){
    INC();
    SBC();
}
void CPU::LAS(){
    A = X = S = bus->readAddress(memory) & S;
    checkN(A);
    checkZ(A);
}
void CPU::LAX(){
    LDA();
    LDX();
}
void CPU::RLA(){
    ROL();
    AND();
}
void CPU::RRA(){
    ROR();
    ADC();
}
void CPU::SAX(){
    bus->writeAddress(memory, A & X);
}
void CPU::SHX(){
    bus->writeAddress(memory, X & ((unsigned char)(memory >> 8)) + 1);
}
void CPU::SHY(){
    bus->writeAddress(memory, Y & (memory >> 8));
}
void CPU::SLO(){
    ASL();
    ORA();
}
void CPU::SRE(){
    LSR();
    EOR();
}
void CPU::TAS(){
    
}
void CPU::XAA(){
    A = X & bus->readAddress(memory);
    checkN(A);
    checkZ(A);
}


void CPU::Addition(unsigned short Oper2){
    unsigned short Oper1 = A;
    unsigned short result = Oper1 + Oper2 + (getC() ? 1 : 0);
    A = (unsigned char)result;
    A == 0 ? setZ() : clearZ();
    A & 0x80 ? setN() : clearN();
    result > 0xFF ? setC() : clearC();
    //test overflow 1
    (Oper1 ^ result ) & (Oper2 ^ result) & 0x80 ? setV() : clearV();
}
void CPU::Branch(){
    unsigned short oldPC = PC;
    PC = memory;
    if((oldPC ^ PC) & 0xFF00) newCycle();
    newCycle();
    
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
    // printf(" Reached an Illegal OPCODE that is not defined.");
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
    if(memory >= 0x4020 && memory < 0x6000){
        memory = PC - 1;
        bus->last_value = high;
    }
    // bus->readAddress(memory);
}
void CPU::absX(){
    unsigned short low = bus->readAddress(PC++);
    unsigned short high = bus->readAddress(PC++);
    memory = (high << 8) | low;
    if(((memory + X) & 0xFF00) != (memory & 0xFF00)){ //page cross
        bus->readAddress((memory & 0xFF00) + ((memory + X) & 0xFF));
        if((opcode != 0x9D) && ((opcode & 0x0F) != 0x0E)){ 
            newCycle();
        }
    } 
    memory += X;
    // bus->readAddress(memory);
}
void CPU::absY(){
    unsigned short low = bus->readAddress(PC++);
    unsigned short high = bus->readAddress(PC++);
    memory = (high << 8) | low;
    if(((memory + Y) & 0xFF00) != (memory & 0xFF00)){ //page cross
        bus->readAddress((memory & 0xFF00) + ((memory + Y) & 0xFF));
        if(opcode != 0x99){ 
            newCycle();
        }
    } 
    memory += Y;
    // bus->readAddress(memory);
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
    // bus->readAddress(memory);
}
void CPU::indY(){
    unsigned char memIndirect = bus->readAddress(PC++);
    unsigned short low = bus->readAddress(memIndirect++); //memIndirect must be < 0xFF
    unsigned short high = bus->readAddress(memIndirect);
    memory = (high << 8) | low;
    if(((memory + Y) & 0xFF00) != (memory & 0xFF00)){ //page cross
        bus->readAddress((memory & 0xFF00) + ((memory + Y) & 0xFF));
        if(opcode != 0x91 ){
            newCycle();
        }
    } 
    memory += Y;
    // bus->readAddress(memory);
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
    if(nmi_pin){
        nmi_pin = false;
        nmi();
    }else if(bus->cartridge->I || bus->apu->F || bus->apu->I){
        if(!getI()){
            irq();
        }
    }
    for(int i=0;i<instructions[opcode].cycles;i++){
        newCycle();
    }

    unsigned short opc = PC;
    opcode = bus->readAddress(PC++);
    invoke(instructions[opcode].address_mode, *this);
    invoke(instructions[opcode].instruction, *this);
}

void CPU::powerON(){
    P = 0x34;
    A = X = Y = 0;
    S = 0xFD;
    PC = 0x8000;
    total_cycles = 0;
    isAccumulator = irq_pin = nmi_pin = false;
    for(int j=0;j<0x800;j++) RAM[j] = 0;
}

void CPU::reset(){
    setI();
    S -= 3;
    P |= 0x04;
    PC = bus->readAddress(0xFFFD) << 8;
    PC |= bus->readAddress(0xFFFC);
}

void CPU::irq(){
    for(int i=0;i<7;i++) newCycle();
    unsigned char low = PC;
    unsigned char high = PC >> 8;
    Push(high);
    Push(low); 
    clearB();
    Push(P);
    setI();
    if(nmi_pin){
        nmi_pin = false;
        PC = bus->readAddress(0xFFFB) << 8;
        PC |= bus->readAddress(0xFFFA);
    }else{
        PC = bus->readAddress(0xFFFF) << 8;
        PC |= bus->readAddress(0xFFFE);
    }
    
    
}

void CPU::nmi(){
    for(int i=0;i<7;i++) newCycle();
    unsigned char low = PC;
    unsigned char high = PC >> 8;
    Push(high);
    Push(low); 
    clearB();
    Push(P);
    setI();
    PC = bus->readAddress(0xFFFB) << 8;
    PC |= bus->readAddress(0xFFFA);
    
}

void CPU::connectBus(Bus *bus){
    this->bus = bus;
}

void invoke(void (CPU::*function)(), CPU &obj) {
    (obj.*function)();
}

void CPU::setirq(bool value){
    irq_pin = value;
}

void CPU::setnmi(){
    nmi_pin = true;
}

void CPU::newCycle(){
    total_cycles++;
    bus->clockAPU();
    for(int j=0;j<3;j++) bus->movePPU();
}


unsigned char CPU::readAddress(unsigned short address){
    return RAM[address];
}
void CPU::writeAddress(unsigned short address, unsigned char value){
    RAM[address] = value;
}