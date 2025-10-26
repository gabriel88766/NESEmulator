//Warning:
//This APU is not sending samples based on cycles
//That means if a game change the samples mid frame, it should
//not be played. 
#include "apu.h"
#include <iostream>
#include <math.h>

APU::APU(){
    reset();
}

void APU::reset(){
    for(int i=0;i<0x20;i++) reg[i] = 0;
    cnt = 0;
    for(int i=0;i<5;i++){
        tim[i] = 1;
        startFlag[i] = en[i] = false;
        phase[i] = len[i] = dvswp[i] = evp[i] = dvevp[i] = 0;
    }
    for(int j=0;j<4096;j++){
        rng[j] = (rand() % 32768) > 16383 ? 1 : 0;
    }
    noise = 1;
    len2  = cntdmc = size = addr = 0;
    outp = 0;
    relT = F = I = false;
}

void APU::connectBus(Bus *bus){
    this->bus = bus;
}



unsigned char APU::readMemory(unsigned short address){
    if(address == 0x15){
        unsigned char ans = 0;
        for(int j=0;j<=4;j++){
            if(en[j] && (len[j] >= 1)) ans |= (1 << j);
        }
        if(F){
             ans |= (1 << 6);
             F = false;
        }
        if(I) ans |= (1 << 7);
        return ans;
    }else return 0;//should not happen
}   
void APU::writeMemory(unsigned short address, unsigned char value){
    reg[address] = value;
    //sweep
    if(address == 1 || address == 5){    
        dvswp[address >> 2] = (((value >> 4) & 7) + 1);
    }
    if(address == 2 || address == 3) reltim[0] = ((reg[3] & 7) << 8) + reg[2];
    if(address == 6 || address == 7) reltim[1] = ((reg[7] & 7) << 8) + reg[6];
    if(address <= 0xF && ((address & 3) == 3)){
        if(en[address >> 2]){
            len[address >> 2] = len_table[value >> 3];
        }
        if(address == 3 || address == 7 || address == 0xF){
            startFlag[address >> 2] = true;
        }
        if(address == 3 || address == 7){
            // phase[address >> 2] = 0;
            // tim[address >> 2] = 2*(reltim[address >> 2]+1);
        }
        if(address == 0xB){
            relT = true;
        }
    }
    
    if(address == 0x8){
        if(value & 0x80) relT = true;
    }
    
    //DMC:
    if(address & 0x10){
        if(!(value & 0x80)) I = false;  
    }
    if(address == 0x11) outdmc = reg[address] & 0x7F;
    if(address == 0x15){
        I = false;
        if(value & (1 << 4)){
            if(len[4] == 0){
                restart_dmc = true;
            }
            en[4] = true;
        }else{
            size = 0;
            len[4] = 0;
            enddmc = true;
            en[4] = false;
            restart_dmc = false;
        }
        for(int j=0;j<=3;j++){
            if(value & (1 << j)){
                en[j] = true;
            }else{
                en[j] = false;
                len[j] = 0;
            }
        }
    }

    if(address == 0x17){
        if(value & 0x40){
            F = false;
        } 
        cnt = 0;
        if(value & 0x80){
            sweep();
            lenCounter();
            linearCounter();
            envelope();
        }
    }
}

void APU::tick(){
    if(reg[0x17] & 0x80){
        if(cnt == CYCLES[1] || cnt == CYCLES[4]){
            //Length counter and sweep
            sweep();
            lenCounter();
        }
        if(cnt != CYCLES[3]){
            //Envelope and linear counter
            envelope();
            linearCounter();
        }
        if(cnt == CYCLES[4]){
            cnt = 0;
        }
    }else{
        //Envelope and linear counter
        linearCounter();
        envelope();
        if(cnt == CYCLES[1] || cnt == CYCLES[3]){
            //Length counter and sweep
            sweep();
            lenCounter();
        }
        if(cnt == CYCLES[3]){
            //interruption
            if(!(reg[0x17] & 0x40)){
                F = true;
            }
            cnt = 0;
        }
    }
    if(relT) {
        len2 = (reg[0x8] & 0x7F);
        if(!(reg[0x8] & 0x80)) relT = false;
    }
}

void APU::clockdmc(){
    shiftdmc++;
    if(shiftdmc == 8){
        shiftdmc = 0;
        if(len[4] >= 1){
            len[4]--;
            addr++;
            if(addr == 0) addr += 0x8000;
            if(len[4] == 0){
                if(reg[0x10] & 0x40){
                    restart_dmc = true;
                }else if(reg[0x10] & 0x80){
                    I = true;
                }
            }else{
                for(int i=0;i<3;i++) bus->cpu->newCycle();
            }
            buffer_dmc = bus->readAddress(addr);
        }else enddmc = true;
    }
    if(en[4] && (!enddmc)){
        if(buffer_dmc & (1 << shiftdmc)) outdmc += 2;
        else outdmc -= 2;
        if(outdmc <= 1) outdmc = 1;
        if(outdmc >= 127) outdmc = 127;
    }
}

void APU::clock(){
    cnt++;
    double aux2 = 0;
    double aux = 1e-300;
    { //pulses
        for(int i=0;i<2;i++){
            if(!en[i]) continue;
            if(!len[i]) continue;
            if(reltim[i] < 8 || reltim[i] > 0x7FF) continue;
            tim[i]--;
            if(tim[i] == 0){
                if(phase[i] == 7) phase[i] = 0;
                else phase[i]++;
                tim[i] = 2*(reltim[i]+1);
            }
            
            
            int df = (reg[4*i] & 0xC0) >> 6; 
            int vol = (reg[4*i] & 0x10 ? reg[4*i] & 0xF : evp[i]);
            aux += (vol * pulseVal[df][phase[i]]);
        }
    }
    aux2 += (95.88 / (8128.0/aux + 100));
    aux = 1e-300;
    {//triangle
        int t = ((reg[0xB] & 7) << 8) + reg[0xA];
        if(en[2] && len[2] && len2 && t >= 4){
            aux += lvt = triangle[phase[2]] / 8227.0;
            tim[2]--;
            if(tim[2] == 0){
                if(phase[2] == 31) phase[2] = 0;
                else phase[2]++;
                tim[2] =  ((((reg[0xB] & 7) << 8) + reg[0xA]) + 1);
            }
        }else aux += lvt;
    } 
    {//noise
        tim[3]--;
        if(tim[3] == 0){
            tim[3] = noise_table[reg[0xE] & 0xF];
            int feedback;
            if(reg[0xE] & 0x80){
                feedback = (noise & 1) ^ ((noise & 0x40) >> 6);
            }else{
                feedback = (noise & 1) ^ ((noise & 2) >> 1);
            }
            noise >>= 1;
            noise |= feedback << 14;
        }
        int vol = (reg[0xC] & 0x10 ? reg[0xC] & 0xF : evp[3]);
        if((en[3]) && (len[3])){
            if(!(noise & 1)) aux += vol/12241.0;
        }
    }
    {//DMC
        aux += outdmc/22638.0;
    }
    aux2 += 159.79 / (1.0/aux + 100);
    samples.push_back(aux2);
    for(int j=0;j<5;j++){
        if(CYCLES[j] == cnt) tick();
    }
    if(restart_dmc){
        restart_dmc = false; 
        size = 8*((reg[0x13] << 4) + 1);
        addr = 0xC000 + 64*reg[0x12];
        if(enddmc){
            len[4] = (reg[0x13] << 4);
            buffer_dmc = bus->readAddress(addr);
        }
        else len[4] = ((reg[0x13] << 4) + 1);
        phase[4] = 0;
        
        enddmc = false;
    }
    if(cntdmc != 0) cntdmc--;//It's correct, don't change!!!
    if(cntdmc == 0){
        cntdmc = dmc_table[reg[0x10] & 0xF] + 0.01; //It's correct, don't change!!!
        clockdmc();   
    }
    
}

void APU::linearCounter(){
     if(len2 != 0) len2--;
}

void APU::envelope(){
    for(int j=0;j<=3;j++){
        if(j == 2) continue;
        if(startFlag[j]){
            startFlag[j] = false;
            evp[j] = 15;
            dvevp[j] = (reg[4*j] & 0xF);
        }else{
            if(dvevp[j] == 0){
                dvevp[j] = (reg[4*j] & 0xF);
                if(evp[j] != 0) evp[j]--;
                else{
                    if(reg[4*j] & 0x20) evp[j] = 15;
                }
            }else{
                dvevp[j]--;
            }
        }
    }
}

void APU::sweep(){
    for(int j=0;j<=1;j++){
        if(reg[4*j+1] & 0x80){
            dvswp[j]--;
            if(dvswp[j] == 0){
                dvswp[j] = (((reg[4*j+1] >> 4) & 7) + 1);
                int s = reg[4*j+1] & 7;
                if(s && reltim[j] >= 8 && reltim[j] <= 0x7FF){
                    if(reg[4*j+1] & 8){
                        reltim[j] -= (reltim[j] >> s);
                        if(j == 0) reltim[j]--;
                    }else{
                        reltim[j] += (reltim[j] >> s);
                    }
                }
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
