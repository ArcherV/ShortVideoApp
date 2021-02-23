//
// Created by ArcherV on 2021-02-22.
//

#ifndef SHORTVIDEOAPP_XUTILS_H
#define SHORTVIDEOAPP_XUTILS_H

#include <android/asset_manager.h>

extern int WIDTH;
extern int HEIGHT;

struct AVFrame;

#define PI 3.1415926f
#define normalize(x, y, z)                  \
{                                               \
        float norm = 1.0f / sqrt(x*x+y*y+z*z);  \
        x *= norm; y *= norm; z *= norm;        \
}
#define I(_i, _j) ((_j)+4*(_i))
#define y(r,g,b)  (((66 * r + 129 * g + 25 * b + 128) >> 8) + 16)
#define u(r,g,b)  (((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128)
#define v(r,g,b)  (((112 * r - 94 * g - 18 * b + 128) >> 8) + 128)
#define color(x)  ((unsigned char)((x < 0) ? 0 : ((x > 255) ? 255 : x)))
#define RGBA_YUV420P 0x00014012

char *readAssetFile(const char *, AAssetManager *);
void rgbToFrame(int width, int height, unsigned char *rgb, AVFrame *frame, int type);
void matrixSetIdentityM(float *);
void matrixSetRotateM(float *, float, float, float, float);
void matrixMultiplyMM(float *, float *, float *);
void matrixRotateM(float *, float, float, float, float);

#endif //SHORTVIDEOAPP_XUTILS_H
