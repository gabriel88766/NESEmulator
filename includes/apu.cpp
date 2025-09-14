#include "apu.h"
#include <iostream>

APU::APU(){
    reset();
}

void APU::reset(){
    for(int i=0;i<0x20;i++) reg[i] = 0;
    tc = 0;
    for(int i=0;i<5;i++){
        en[i] = true;
        if(i < 3) phase[i] = 0;
        if(i < 4) len[i] = 0;
    }
    F = I = false;
}

void APU::connectBus(Bus *bus){
    this->bus = bus;
}



unsigned char APU::readMemory(unsigned short address){
    if(address == 0x4015){
        unsigned char ans = 0;
        for(int j=0;j<=4;j++){
            if(en[j] && len[j]) ans |= (1 << j);
        }
        if(!(reg[0x17] & 0x40)) ans |= (1 << 6);
        if(I) ans |= (1 << 7);
        return ans;
    }else return 0;//should not happen
}   
void APU::writeMemory(unsigned short address, unsigned char value){
    reg[address] = value;
    if(address == 0x15){
        for(int j=0;j<=4;j++){
            if(value & (1 << j)){
                en[j] = true;
            }else{
                en[j] = false;
                len[j] = 0;
            }
        }
    }
    if(address <= 0xF && (address & 3 == 3) && en[address >> 2]){
        len[address >> 2] = len_table[value >> 3];
        if(address == 3 || address == 7) phase[address >> 2] = 0;
    }
}

void APU::tick(){
    tc++;
    if(reg[0x17] & 0x80){
        if(tc == 2 || tc == 5){
            //Length counter and sweep
            sweep();
            lenCounter();
        }
        if(tc != 4){
            //Envelope and linear counter
        }


        if(tc == 5) tc = 0;
    }else{
        //Envelope and linear counter
        if(tc == 2 || tc == 4){
            //Length counter and sweep
            sweep();
            lenCounter();
        }
        if(tc == 4){
            //interruption
            
            if(!(reg[0x17] & 0x40)){
                bus->setIRQ();
            }
            tc = 0;
        }
    }
}


void APU::sweep(){

}

void APU::lenCounter(){
    for(int v = 0; v <= 3; v++){
        if(!en[v]) continue;
        bool halt;
        if(v == 2) halt = reg[8] & 0x80 ? true : false;
        else halt = reg[4*v] & 0x20 ? true : false; 
        if(halt) continue;
        if(len[v] == 0) continue;
        len[v]--;
        //if en[v] = 0 or len[v] = 0 or VVVV = 0 then it's silenced
    }
}

//This function will give the exact sampling 
//summing all 4 channels and DMC, which is needed for 
//SDL
//change as your needs!
void APU::getSampling(short *buffer, int length, double rate){
    const int mult = 436;
    const double PI = 3.14159265358979323846;
    const double twoPI = 2.0 * PI;
    const double hPI = PI/2;
    //pulse1 pulse2 triangle
    for(int j=0;j<length;j++) buffer[j] = 0;
    for(int i=0;i<3;i++){
        if(!en[i]) continue;
        if(!len[i]) continue;
        int v = reg[4*i] & 0xF;
        if(i <= 1 && v == 0) continue; //only pulse have volume
        int t = ((reg[4*i+3] & 7) << 8) + reg[4*i+2];
        if(t < 8) continue;
        double freq =  1789773.0/(16.0 * (t+1));
        if(i == 2) freq /= 2;
        double phase_inc =  (twoPI * freq) / rate;
        int df = (reg[4*i] & 0xC0) >> 6; 
        double duty = df * 0.25;
        if(duty < 0.1) duty = 0.125;
        for(int j=0;j<length;j++){
            if(i <= 1){
                if(phase[i] >= twoPI * duty) buffer[j] += mult * v;
                else buffer[j] -= mult * v;    
            }else{
                // continue;
                if(phase[i] < hPI) buffer[j] += 15 * mult * (phase[i])/hPI;
                else if(phase[i] < PI) buffer[j] += 15 * mult * (PI-phase[i])/hPI;
                else if(phase[i] < 3*hPI) buffer[j] -= 15 * mult * (phase[i] - PI)/hPI;
                else buffer[j] -= 15 * mult * (2*PI - phase[i]) / hPI;
            }


            phase[i] += phase_inc;
            if (phase[i] >= twoPI)
                phase[i] -= twoPI;
        }
    }
}