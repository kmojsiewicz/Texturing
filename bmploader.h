#ifndef BMPLOADER_H
#define BMPLOADER_H

#include <stdlib.h>
#include <QPainter>

union TRGBColor {
    struct {
        unsigned char B;
        unsigned char G;
        unsigned char R;
        unsigned char alpha;
    };
    QRgb qRGB;
};

class BMPLoader
{
public:
    BMPLoader();
     static QRgb *loadTexture(const char *fileName, int &width, int &height);
};

#endif // BMPLOADER_H
