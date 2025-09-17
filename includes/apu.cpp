#include "apu.h"
#include <iostream>

APU::APU(){
    reset();
}

void APU::reset(){
    for(int i=0;i<0x20;i++) reg[i] = 0;
    cnt = 0;
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
    if(address == 0x15){
        unsigned char ans = 0;
        for(int j=0;j<=4;j++){
            if(en[j] && len[j]) ans |= (1 << j);
        }
        if(F){
             ans |= (1 << 6);
             F = false;
             bus->setIRQ(false);
        }
        if(I) ans |= (1 << 7);
        return ans;
    }else return 0;//should not happen
}   
void APU::writeMemory(unsigned short address, unsigned char value){
    if(address == 0x00 || address == 0x04){
        if(!(value & 0x10)){
            vol[address >> 2] = 0xF;
        }else vol[address >> 2] = value & 0xF;
    }
    if(address == 0x17){
        if(value & 0x40){
            bus->setIRQ(false);
            F = false;
        } 
        cnt = 0;
        if(value & 0x80){
            F = false;
            bus->setIRQ(false);
            sweep();
            lenCounter();
            linearCounter();
        }
    }
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
    if(address <= 0xF && ((address & 3) == 3) && en[address >> 2]){
        len[address >> 2] = len_table[value >> 3];
        if(address == 3 || address == 7){
             phase[address >> 2] = 0;
             dvp[address >> 2] = ((value >> 4) & 7) + 1;
        }
    }
    if(address == 0x8){
        len2 = value & 0x7F;
    }
}

void APU::tick(){
    if(reg[0x17] & 0x80){
        if(cnt == CYCLES[1] || cnt == CYCLES[4]){
            //Length counter and sweep
            sweep();
            // decreaseVolume();
            lenCounter();
            
        }
        if(cnt != CYCLES[3]){
            //Envelope and linear counter
            linearCounter();
        }

        
        if(cnt == CYCLES[4]){
            
            cnt = 0;
            F = false;
            bus->setIRQ(false);
        }
    }else{
        //Envelope and linear counter
        linearCounter();
        
        if(cnt == CYCLES[1] || cnt == CYCLES[3]){
            //Length counter and sweep
            sweep();
            lenCounter();
            // decreaseVolume();
        }
        if(cnt == CYCLES[3]){
            
            //interruption
            F = false;
            bus->setIRQ(false);
            if(!(reg[0x17] & 0x40)){
                bus->setIRQ(true);
                F = true;
            }
            cnt = 0;
        }
    }
}

void APU::clock(){
    cnt++;
    for(int j=0;j<5;j++){
        if(CYCLES[j] == cnt) tick();
    }
}

void APU::linearCounter(){
    // len2 = 0xFF;
    if(!(reg[8] & 0x80)) len2 = reg[8] & 0x7F;
    else if(len2 != 0) len2--;



    //envelope
    
}

void APU::decreaseVolume(){
    for(int j=0;j<=1;j++){
        if(!(reg[4*j] & 0x10)){
            //v decrement'
            if(vol[j] == 0){
                if(reg[4*j] & 0x20){
                    vol[j] |= 0xF;
                }
            }else{
                vol[j]--;
            }
        }
    }
}

void APU::sweep(){
    for(int j=0;j<=1;j++){
        if(reg[4*j+1] & 0x80){
            dvp[j]--;
            if(dvp[j] == 0){
                dvp[j] = ((reg[4*j+1] >> 4) & 7) + 1;
                int s = reg[4*j+1] & 7;
                unsigned short val = ((reg[4*j+3] & 7) << 8) + reg[4*j+2];
                if(reg[4*j+1] & 8){
                    val -= (val >> s);
                    if(j == 0) val--;
                }else{
                    val += (val >> s);
                }
                
                if(val >= (1 << 11)) val = 0;
                reg[4*j+2] = val & 0xFF;
                reg[4*j+3] &= 0xF8;
                reg[4*j+3] |= (val >> 8);
            }
        }
    }
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
    }
}

void APU::Pulse(short *buffer, int length, double rate, int num){
    if(!en[num]) return;
    if(!len[num]) return;
    int t = ((reg[4*num + 3] & 7) << 8) + reg[4*num+2];
    if(t < 8) return;
    double freq =  1789773.0/(16.0 * (t+1));
    double phase_inc =  (twoPI * freq) / rate;
    int df = (reg[4*num] & 0xC0) >> 6; 
    double duty = df * 0.25;
    if(df == 0) duty = 0.125;
    for(int j=0;j<length;j++){
        if(phase[num] >= twoPI * duty) buffer[j] += mult * vol[num];
        else buffer[j] -= mult * vol[num];    
        phase[num] += phase_inc;
        if (phase[num] >= twoPI)
            phase[num] -= twoPI;
    }
}

void APU::Pulse1(short *buffer, int length, double rate){
    Pulse(buffer, length, rate, 0);
}

void APU::Pulse2(short *buffer, int length, double rate){
    Pulse(buffer, length, rate, 1);
}

void APU::Triangle(short *buffer, int length, double rate){
    if(!en[2]) return;
    if(!len[2]) return;
    if(len2) return;
    int t = ((reg[0xB] & 7) << 8) + reg[0xA];
    double freq =  1789773.0/(32.0 * (t+1));
    double phase_inc =  (twoPI * freq) / rate;
    for(int j=0;j<length;j++){
        if(phase[2] < hPI) buffer[j] += 15 * mult * (phase[2])/hPI;
        else if(phase[2] < PI) buffer[j] += 15 * mult * (PI-phase[2])/hPI;
        else if(phase[2] < 3*hPI) buffer[j] -= 15 * mult * (phase[2] - PI)/hPI;
        else buffer[j] -= 15 * mult * (2*PI - phase[2]) / hPI;
        phase[2] += phase_inc;
        if (phase[2] >= twoPI)
            phase[2] -= twoPI;
    }
}
//This function will give the exact sampling 
//summing all 4 channels and DMC, which is needed for 
//SDL
//change as your needs!
void APU::getSampling(short *buffer, int length, double rate){
    
    //pulse1 pulse2 triangle
    for(int j=0;j<length;j++) buffer[j] = 0;
    Pulse1(buffer, length, rate);
    Pulse2(buffer, length, rate);
    Triangle(buffer, length, rate);
   


    // //normalizing
    // int mx = 0;
    // for(int j=0;j<length;j++){
    //     if(buffer[j] > 0 && buffer[j] > mx) mx = buffer[j];
    //     if(buffer[j] < 0 && -buffer[j] > mx) mx = -buffer[j];
    // }
    // double mtp = 32700.0/mx;
    // for(int j=0;j<length;j++){
    //     buffer[j]  = buffer[j] * mtp;
    // }
}