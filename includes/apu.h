#ifndef APU_H
#define APU_H

#include "bus.h"

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
    double phase[5];
    unsigned char len[5];
    unsigned char vol[5];
    unsigned char len2;
    unsigned char dvp[5];
    unsigned char envp[5];
    double lv2;
    double rng[4096];
    double aux[4096];
    double aux2[4096];
    const int CYCLES[5] = {7456, 14912, 22370, 29828, 37280};
    const double dmc_table[16] = {428, 380, 340, 320, 286, 254, 226, 214, 190, 160, 142, 128, 106,  84,  72,  54};
    const double noise_table[16] = {4, 8, 16, 32, 64, 96, 128, 160, 202, 254, 380, 508, 762, 1016, 2034, 4068};
    const unsigned char len_table[32] = {10, 254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14, 12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30};
    //for DMC
    int remain;
    int cntdmc;

    int size;
    int addr;
    double outp;


public:
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
    void Pulse(double *buffer, int length, double rate, int num);
    void Pulse1(double *buffer, int length, double rate);
    void Pulse2(double *buffer, int length, double rate);
    void Triangle(double *buffer, int length, double rate);
    void Noise(double *buffer, int length, double rate);
    void DMC(double *buffer, int length, double rate);
    void getSampling(unsigned short *buffer, int length, double rate);
};



#endif