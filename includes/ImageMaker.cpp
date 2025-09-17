#include "ImageMaker.h"
#include <fstream>
#include <cstring>

const unsigned char alpha = 0xFF;

Image::~Image(){
    delete[] Header;
    delete[] data;
}

void Image::makeImage(unsigned width, unsigned height){
    this->width = width;
    this->height = height;
    Header = new unsigned char[0x36];
    memset(Header, 0, 0x36);
    unsigned size = 0x36 + 4 * width * height;
    unsigned sizedata = size - 0x36;
    Header[0] = 0x42;
    Header[1] = 0x4D;
    memcpy(Header + 2, &size, 4);
    Header[10] = 0x36;
    Header[14] = 0x28;
    memcpy(Header + 18, &width, 4);
    memcpy(Header + 22, &height, 4);
    Header[26] = 0x01;
    Header[28] = 0x20;
    Header[30] = 0x00;
    memcpy(Header + 34, &sizedata, 4);
    data = new unsigned char[width*height*4];
}

void Image::setPixel(int x, int y, Color c){
    int location = 4 * ((height - y - 1) * width + x);
    data[location] = c.B;
    data[location+1] = c.G;
    data[location+2] = c.R;
    data[location+3] = alpha; 
}

void Image::writeImage(const char *filename){
    std::ofstream outputImage(filename, std::ios::binary);
    outputImage.write((char *)Header, 0x36);
    outputImage.write((char *)data, 4*width*height);
}