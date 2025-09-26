#ifndef IMAGE_MAKER_H
#define IMAGE_MAKER_H

struct Color{
    unsigned char R;
    unsigned char G;
    unsigned char B;
    //Alpha default FF
};

class Image{//Only RGB 
private:
    unsigned char *Header;
    unsigned char *data;
    unsigned width;
    unsigned height;
public:
    Image(){}
    ~Image();
    void makeImage(unsigned width, unsigned height);
    void setPixel(int x, int y, Color c);
    void writeImage(const char *filename);
};

#endif