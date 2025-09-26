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
        en[i] = false;
        phase[i] = len[i] = vol[i] = dvp[i] = envp[i] = 0;
    }
    for(int j=0;j<4096;j++){
        rng[j] = (rand() % 32768) > 16383 ? 1 : 0;
    }
    len2 = remain = cntdmc = size = addr = 0;
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
            if(en[j] && len[j]) ans |= (1 << j);
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
    if(address == 0x00 || address == 0x04 || address == 0xC){
        
        if(!(value & 0x10)){
            vol[address >> 2] = 0xF;
            envp[address >> 2] = (value & 0xF) + 1; 
        }else vol[address >> 2] = value & 0xF;
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
    if(address == 0x15){
        I = false;
        if(value &(1 << 4)){
            if(len[4] <= 8){
                phase[4] = 0;
                len[4] += size = 8*((reg[address] << 4) + 1);
                cntdmc = dmc_table[reg[0x10] & 0xF];
            }
            en[4] = true;
        }else{
            size = 0;
            len[4] %= 8;
            en[4] = false;
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
    //DMC:
    if(address & 0x10){
        if(!(value & 0x80)) I = false;  
    }
    if(address == 0x11) outp = reg[address] & 0x7F;
    if(address == 0x12) addr = 0xC000 + 64*reg[address];
    if(address == 0x13){
       
        len[4] = size = 8*((reg[address] << 4) + 1);
        cntdmc = dmc_table[reg[0x10] & 0xF];
    }


    if(address == 1 || address == 5){
        
        if(!(reg[(address >> 2) << 2] & 0x10)) vol[address >> 2] = 0xF;
        else vol[address >> 2] = (reg[(address >> 2) << 2] & 0xF);
        dvp[address >> 2] = (((value >> 4) & 7) + 1);
    }
    if(address <= 0xF && ((address & 3) == 3) && en[address >> 2]){
        len[address >> 2] = len_table[value >> 3];
        if(address == 3 || address == 7 || address == 0xF){
            if(!(reg[address - 3] & 0x10)){
                vol[address >> 2] = 0xF;
                envp[address >> 2] = (reg[address - 3] & 0xF);   
                phase[address >> 2] = 0;
            }
        }
        if(address == 0xB){
            relT = true;
        }
    }
    
    if(address == 0x8){
        if(value & 0x80) relT = true;
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

void APU::clock(){
    cnt++;
    if(cntdmc != 0) cntdmc--;
    if(cntdmc == 0){
        cntdmc = dmc_table[reg[0x10] & 0xF];
        
        if(len[4] >= 1){
            len[4]--;
            if(len[4] % 8 == 0){
                bus->cpu->newCycle(); bus->cpu->newCycle(); bus->cpu->newCycle();
            }
            if(len[4] == 0){
                if(reg[0x10] & 0x40){
                    len[4] = (reg[0x13] << 4) + 1;
                }else if(reg[0x10] & 0x80){
                    I = true;
                }
            }
        }
    }
    
    for(int j=0;j<5;j++){
        if(CYCLES[j] == cnt) tick();
    }

}

void APU::linearCounter(){
     if(len2 != 0) len2--;
}

void APU::envelope(){
    for(int j=0;j<=3;j++){
        if(j == 2) continue;
        if(!(reg[4*j] & 0x10)){
            //v decrement'
            
            if(envp[j] == 0){
                envp[j] = (reg[4*j] & 0xF);  
                if(vol[j] == 0){
                    if(reg[4*j] & 0x20) vol[j] |= 0xF;
                }else{
                    vol[j]--;
                }
            }else envp[j]--;
        }
    }
}

void APU::sweep(){
    for(int j=0;j<=1;j++){
        if(reg[4*j+1] & 0x80){
            dvp[j]--;
            if(dvp[j] == 0){
                dvp[j] = (((reg[4*j+1] >> 4) & 7) + 1);
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

void APU::Pulse(double *buffer, int length, double rate, int num){
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
        if(phase[num] >= twoPI * duty) buffer[j] += vol[num]; 
        phase[num] += phase_inc;
        if (phase[num] >= twoPI)
            phase[num] -= twoPI;
    }
}

void APU::Pulse1(double *buffer, int length, double rate){
    Pulse(buffer, length, rate, 0);
}

void APU::Pulse2(double *buffer, int length, double rate){
    Pulse(buffer, length, rate, 1);
}

void APU::Triangle(double *buffer, int length, double rate){
    bool isen = true;
    if(!en[2]) isen = false;
    if((!len[2]) || (!len2)) isen = false;
    int t = ((reg[0xB] & 7) << 8) + reg[0xA];
    if(t < 8) isen = false;
    if(!isen){
        for(int j=0;j<length;j++) buffer[j] += lv2;
        return;
    }
    
    double freq =  1789773.0/(32.0 * (t+1));
    double phase_inc =  (twoPI * freq) / rate;
    for(int j=0;j<length;j++){
        if(isen){
            //converting to int makes it sound more NES like.
            buffer[j] += lv2 = ( (int)(15.0  * fabs((phase[2] - PI)/PI))/8227.0);
        }
        phase[2] += phase_inc;
        if (phase[2] >= twoPI)
            phase[2] -= twoPI;
        
    }
}

void APU::Noise(double *buffer, int length, double rate){
    if((!en[3]) || (!len[3])){
        return;
    }
    double inc_phase = (1789773.0 / noise_table[reg[0xE] & 0xF]) / rate;
    for(int j=0;j<length;j++){
        buffer[j] += (vol[3] * rng[(int)phase[3]])/12241.0;
        phase[3] += inc_phase;
        if(phase[3] >= 4096.0) phase[3] -= 4096.0;
    }
}

void APU::DMC(double *buffer, int length, double rate){
    double inc_phase = (1789773.0/dmc_table[reg[0x10] & 0xF])  / rate;
    for(int j=0;j<length;j++){
        if(en[4]){
            int curv = (int)phase[4];
            double lf = phase[4];
            phase[4] += inc_phase;
            int nextv = (int)phase[4];
            if(phase[4] >= size) {
                //loop?
                phase[4] = 0;
                if(!(reg[0x10] & 0x40)) {
                    size = 0;
                    // outp = reg[0x11] & 0x7F; //testing
                }
            }else{
                unsigned short t1 = addr + curv/8;
                unsigned short t2 = addr + nextv/8;
                if(t1 < 0x8000) t1 += 0x8000;
                if(t2 < 0x8000) t2 += 0x8000;
                int v1 = bus->readAddress(t1) & (1 << (curv % 8));
                int v2 = bus->readAddress(t2) & (1 << (nextv % 8));
                if(curv == nextv ){
                    if(v1) outp += 2 * (phase[4] - lf);
                    else outp -= 2 * (phase[4] - lf);
                }else{
                    if(v1) outp += 2 * (nextv - lf);
                    else outp -= 2 * (nextv - lf);
                    if(v2) outp += 2 * (phase[4] - nextv);
                    else outp -= 2 * (phase[4] - nextv);
                }
                if(outp >= 127) outp = 127;
                if(outp <= 1) outp = 1;
            }
        }

        buffer[j] += outp / 22638.0;
    }
    // int len = 
}
//change as your needs!
void APU::getSampling(unsigned short *buffer, int length, double rate){
    
    for(int j=0;j<length;j++) aux[j] = aux2[j] = 1e-9;
    Pulse1(aux, length, rate);
    Pulse2(aux, length, rate);
    for(int j=0;j<length;j++){
        aux2[j] += 95.88 / (8128.0/aux[j] + 100);
    }
    for(int j=0;j<length;j++) aux[j] = 1e-9;
    Triangle(aux, length, rate);
    Noise(aux, length, rate);
    DMC(aux, length, rate);
    for(int j=0;j<length;j++){
        aux2[j] += 159.79 / (1.0/aux[j] + 100);
    }
    
    for(int j=0;j<length;j++){
        buffer[j] = aux2[j] * 65535;
    }

}