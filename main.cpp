#include <QApplication>
#include <QLabel>
#include <QPainter>
#include <QTimer>
#include <QtMath>
#include "QDebug"

#include "bmploader.h"
#include "texture.h"

#define WND_WIDTH   800
#define WND_HEIGHT  600

template <class T>
void swap_data(T& x, T& y)
{
    T temp;
    temp = x;
    x = y;
    y = temp;
}

struct TVertex {
    float x;
    float y;
    float z;
    float u;
    float v;
};

inline void putPixel(int x, int y, QRgb col, QPainter *painter)
{
    painter->setPen(col);
    painter->drawPoint(x, y);
}

void drawTriangle(TVertex V1, TVertex V2, TVertex V3, Texture *texture, QPainter *painter)
{
    if (V1.y > V2.y) {                                                  // sort the vertices (V1,V2,V3) by their Y values
        swap_data(V1, V2);
    }
    if (V1.y > V3.y) {
        swap_data(V1, V3);
    }
    if (V2.y > V3.y) {
        swap_data(V2, V3);
    }

    if ((int)V1.y == (int)V3.y) return;                                 // check if we have more than a zero height triangle

    // We have to decide whether V2 is on the left side or the right one. We could do that by findng the V4, and
    // check the disatnce from V4 to V2 (V4.y = V2.y). V4 is one the edge (V1V3)
    // Using formula : I(t) = A + t(B-A) we find y = V1.y + t(V3.y - V1.y) and y = V2.y
    // V2.y - V1.y = t(V3.y - V1.y) -->  t = (V2.y - V1.y)/(V3.y - V1.y)
    // V4.x = V1.x + t(V3.x - V1.x) and V4.y = V2.y
    // float distance = V4.x - V2.x
    // if (distance > 0) then the middle vertex is on the left side (V1V3 is the longest edge on the right side)

    float dY21 = 1.0 / ceil(V2.y - V1.y);
    float dY31 = 1.0 / ceil(V3.y - V1.y);
    float dY32 = 1.0 / ceil(V3.y - V2.y);

    float dXdY21 = (float)(V2.x - V1.x) * dY21;                         // dXdY means deltaX/deltaY
    float dXdY31 = (float)(V3.x - V1.x) * dY31;
    float dXdY32 = (float)(V3.x - V2.x) * dY32;
    float dXdY31tmp = dXdY31;

    float dX = 1.0 / ((V3.x - V1.x)*ceil(V2.y - V1.y) + (V1.x - V2.x)*ceil(V3.y - V1.y));

    // we calculate delta values ​​to find the z value
    float dZdY21 = (float)(V2.z - V1.z) * dY21;
    float dZdY31 = (float)(V3.z - V1.z) * dY31;
    float dZdY32 = (float)(V3.z - V2.z) * dY32;
    float dZdX   = (float)((V3.z - V1.z)*ceil(V2.y - V1.y) + (V1.z - V2.z)*ceil(V3.y - V1.y)) * dX;

    // we calculate delta values ​​to find the u-value of the texture
    float dUdY21 = (float)(V2.u - V1.u) * dY21 * (texture->width - 1);
    float dUdY31 = (float)(V3.u - V1.u) * dY31 * (texture->width - 1);
    float dUdY32 = (float)(V3.u - V2.u) * dY32 * (texture->width - 1);
    float dUdX   = (float)((V3.u - V1.u)*ceil(V2.y - V1.y) + (V1.u - V2.u)*ceil(V3.y - V1.y)) * dX * (texture->width - 1);

    // we calculate delta values ​​to find the v-value of the texture
    float dVdY21 = (float)(V2.v - V1.v) * dY21 * (texture->height - 1);
    float dVdY31 = (float)(V3.v - V1.v) * dY31 * (texture->height - 1);
    float dVdY32 = (float)(V3.v - V2.v) * dY32 * (texture->height - 1);
    float dVdX   = (float)((V3.v - V1.v)*ceil(V2.y - V1.y) + (V1.v - V2.v)*ceil(V3.y - V1.y)) * dX * (texture->height - 1);

    int x;
    int x_end;
    float x_left = V1.x;
    float x_right = V1.x;
    float y = V1.y;
    float z, u, v;
    float zp = V1.z;
    float up = V1.u * (texture->width - 1);
    float vp = V1.v * (texture->height - 1);

    if (dXdY21 > dXdY31) {
        swap_data(dXdY21, dXdY31);
        dZdY21 = dZdY31;
        dUdY21 = dUdY31;
        dVdY21 = dVdY31;
    }

    while (y < V2.y) {
        x_left  += dXdY21;
        x_right += dXdY31;
        x_end    = ceil(x_right);

        zp += dZdY21;
        z = zp;
        up += dUdY21;
        u = up;
        vp += dVdY21;
        v = vp;

        if (x_left < x_right) {
            for (x=ceil(x_left); x<x_end; x++) {
                z += dZdX;
                u += dUdX;
                v += dVdX;
                putPixel(x, y, texture->data[(int)u + (int)v*256], painter);
            }
        }
        else {
            x_end = x_left;
            for (x=ceil(x_right); x<x_end; x++) {
                z += dZdX;
                u += dUdX;
                v += dVdX;
                putPixel(x, y, texture->data[(int)u + (int)v*256], painter);
            }
        }
    y += 1.0;
    }

    dXdY31 = dXdY31tmp;
    if (dXdY32 < dXdY31) {
        swap_data(dXdY31, dXdY32);
        dZdY32 = dZdY31;
        dUdY32 = dUdY31;
        dVdY32 = dVdY31;
    }

    while (y < V3.y) {
        x_left  += dXdY32;
        x_right += dXdY31;
        x_end    = ceil(x_right);

        zp += dZdY32;
        z = zp;
        up += dUdY32;
        u = up;
        vp += dVdY32;
        v = vp;

        for (x=ceil(x_left); x<x_end; x++) {
            z += dZdX;
            u += dUdX;
            v += dVdX;
            putPixel(x, y, texture->data[(int)u + (int)v*256], painter);
        }
        y += 1.0;
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QPixmap pixmap(WND_WIDTH, WND_HEIGHT);
    QPainter painter(&pixmap);
    QLabel windowLabel;

    TVertex A = {  40,  10, 0.0, 0.0, 0.0 };
    TVertex B = { 290, 220, 0.0, 1.0, 1.0 };
    TVertex C = {  80, 280, 0.0, 0.0, 1.0 };

    TVertex D = {  40,  10, 0.0, 0.0, 0.0 };
    TVertex E = { 290, 220, 0.0, 1.0, 1.0 };
    TVertex F = { 270, -10, 0.0, 1.0, 0.0 };

    Texture texture;
    if (texture.loadFromBitmap("posx.bmp")) {
        texture.draw(&painter);
    }

    QTimer t;
    int step = 0;
    QObject::connect(&t, &QTimer::timeout, [&]() {

        TVertex APrim, BPrim, CPrim;
        TVertex DPrim, EPrim, FPrim;
        float matrixRot[2];

        matrixRot[0] = qCos(step * M_PI / 100.0);                       // rotation around the z axis
        matrixRot[1] = qSin(step * M_PI / 100.0);

        APrim = A;
        BPrim = B;
        CPrim = C;
        DPrim = D;
        EPrim = E;
        FPrim = F;
        APrim.x = WND_WIDTH/2  + A.x * matrixRot[0] - A.y * matrixRot[1];
        APrim.y = WND_HEIGHT/2 + A.x * matrixRot[1] + A.y * matrixRot[0];
        BPrim.x = WND_WIDTH/2  + B.x * matrixRot[0] - B.y * matrixRot[1];
        BPrim.y = WND_HEIGHT/2 + B.x * matrixRot[1] + B.y * matrixRot[0];
        CPrim.x = WND_WIDTH/2  + C.x * matrixRot[0] - C.y * matrixRot[1];
        CPrim.y = WND_HEIGHT/2 + C.x * matrixRot[1] + C.y * matrixRot[0];

        DPrim.x = WND_WIDTH/2  + D.x * matrixRot[0] - D.y * matrixRot[1];
        DPrim.y = WND_HEIGHT/2 + D.x * matrixRot[1] + D.y * matrixRot[0];
        EPrim.x = WND_WIDTH/2  + E.x * matrixRot[0] - E.y * matrixRot[1];
        EPrim.y = WND_HEIGHT/2 + E.x * matrixRot[1] + E.y * matrixRot[0];
        FPrim.x = WND_WIDTH/2  + F.x * matrixRot[0] - F.y * matrixRot[1];
        FPrim.y = WND_HEIGHT/2 + F.x * matrixRot[1] + F.y * matrixRot[0];

        pixmap.fill(Qt::black);
        //texture.draw(&painter);
        drawTriangle(APrim, BPrim, CPrim, &texture, &painter);
        drawTriangle(DPrim, EPrim, FPrim, &texture, &painter);
        windowLabel.setPixmap(pixmap);

        ++step;
        //t.stop
    });
    t.start(10);

    windowLabel.setPixmap(pixmap);
    windowLabel.show();

    int ret = a.exec();
    return ret;
}
