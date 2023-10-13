#ifndef TEXTURE_H
#define TEXTURE_H

#include <QPainter>
#include "bmploader.h"

class Texture
{
public:
    QRgb *data;
    int width;
    int height;

    Texture() { width = 0; height = 0; data = NULL; }
    ~Texture() { if (data) delete data; }

    void draw(QPainter *painter);
    int loadFromBitmap(const char *fileName);
    QRgb getColor(int x, int y);
};

#endif // TEXTURE_H
