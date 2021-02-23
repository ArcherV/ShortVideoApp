//
// Created by ArcherV on 2021-02-22.
//

#ifndef SHORTVIDEOAPP_XUTILS_H
#define SHORTVIDEOAPP_XUTILS_H

#include <android/asset_manager.h>

#define PI 3.1415926f
#define normalize(x, y, z)                  \
{                                               \
        float norm = 1.0f / sqrt(x*x+y*y+z*z);  \
        x *= norm; y *= norm; z *= norm;        \
}
#define I(_i, _j) ((_j)+4*(_i))

char *readAssetFile(const char *, AAssetManager *);
void matrixSetIdentityM(float *);
void matrixSetRotateM(float *, float, float, float, float);
void matrixMultiplyMM(float *, float *, float *);
void matrixRotateM(float *, float, float, float, float);

#endif //SHORTVIDEOAPP_XUTILS_H
