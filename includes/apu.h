#ifndef APU_H
#define APU_H

#include "bus.h"
#include <cstdio>
#include <deque>

class Bus;

class APU{
private:

    const int mult = 436;
    const double PI = 3.14159265358979323846;
    const double twoPI = 2.0 * PI;
    const double hPI = PI/2;

    unsigned char reg[0x20];
    Bus *bus;
    int cnt = 0;
    
    bool relT = false;
    bool en[5];
    int phase[5];
    unsigned short len[5]; //len dmc is bigger
    unsigned char len2;
    unsigned char dvswp[5];
    bool swp[2];

    unsigned char evp[5]; //decay level of envelope
    unsigned char dvevp[5]; // divider of envelope.
    bool startFlag[5];
    int reltim[2];
    int tim[5];
    double lvt;
    double rng[4096];
    // double aux[4096];
    // double aux2[4096];
    const int CYCLES[5] = {7456, 14912, 22370, 29830, 37280};
    const double dmc_table[16] = {428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106,  84,  72,  54};
    const double noise_table[16] = {4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068};
    const unsigned char len_table[32] = {10, 254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14, 12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30};
    //for DMC len[4] is bits remaining
    int cntdmc;
    bool restart_dmc = false;
    int shiftdmc = 0;
    bool enddmc = false;
    int size;
    unsigned short addr;
    double outp;
    void clockdmc();
    
    int outdmc;
    unsigned char buffer_dmc;
    //new apu
    int pulseVal[4][8] = {{0, 1, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 0, 0, 0, 0, 0}, {0, 1, 1, 1, 1, 0, 0, 0}, {1, 0, 0, 1, 1, 1, 1, 1}};
    int triangle[32] = {15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15};
    unsigned short noise;
    int remain[2];
public:
    std::deque<float> samples;
    bool F, I;
    APU();
    void reset();
    void connectBus(Bus *bus);
    unsigned char readMemory(unsigned short address);
    void writeMemory(unsigned short address, unsigned char value);
    void tick();
    void clock();
    void linearCounter();
    void envelope();
    void sweep();
    void lenCounter();
};



#endif