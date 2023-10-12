#include "texture.h"

void Texture::draw(QPainter *painter)
{
    int x;
    int y;
    int offset;
    QColor Color;

    if (data == NULL) {
        return;
    }

    offset = 0;
    for (y=0; y<height; y++) {
        for (x=0; x<width; x++) {
            painter->setPen(QColor::fromRgb((QRgb)data[x + offset]));
            painter->drawPoint(x, y);
        }
        offset += width;
    }
}

int Texture::loadFromBitmap(const char *fileName)
{
    if ((data = BMPLoader::loadTexture(fileName, width, height)) != NULL) {
        return 1;
    }

    return 0;
}
