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
    bool F, I;
    bool en[5];
    double phase[5];
    unsigned char len[5];
    unsigned char vol[5];
    unsigned char len2;
    unsigned char dvp[5];
    int CYCLES[5] = {7456, 14912, 22370, 29828, 37280};
    unsigned char len_table[32] = {10, 254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14, 12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30};
public:
    APU();
    void reset();
    void connectBus(Bus *bus);
    unsigned char readMemory(unsigned short address);
    void writeMemory(unsigned short address, unsigned char value);
    void tick();
    void clock();
    void linearCounter();
    void decreaseVolume();
    void sweep();
    void lenCounter();
    void Pulse(short *buffer, int length, double rate, int num);
    void Pulse1(short *buffer, int length, double rate);
    void Pulse2(short *buffer, int length, double rate);
    void Triangle(short *buffer, int length, double rate);
    void getSampling(short *buffer, int length, double rate);
};



#endif