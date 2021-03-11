//
// Created by ArcherV on 2021-02-22.
//

#include <cstdlib>
#include <cstring>
#include "XUtils.h"
#include "Xlog.h"

const int HDR_WIDTH = 2048;
const int HDR_HEIGHT = 1536;
const int HDR_CHANNEL = 3;
const int WIDTH = 720;
const int HEIGHT = 1280;
const AVCodecID VCODEC_ID = AV_CODEC_ID_H264;
const AVCodecID ACODEC_ID = AV_CODEC_ID_AAC;
const int BIT_RATE = 5760000;
const int FRAMERATE = 25;
const int SAMPLE_RATE = 44100;
const int AUDIO_BIT_RATE = 64000;
const int GOP_SIZE = 10;
const int MAX_B_FRAMES = 1;
const AVPixelFormat PIX_FMT = AV_PIX_FMT_YUV420P;
const int PROFILE = FF_PROFILE_H264_BASELINE;
char *vertexShader = nullptr;
char *fragmentOESShader = nullptr;
char *fragmentShader = nullptr;

char *readAssetFile(const char *filename, AAssetManager *mgr) {
    if (mgr == nullptr) {
        XLOGE("readAssetFile", "pAssetManager is null!");
        return nullptr;
    }
    AAsset *pAsset = AAssetManager_open(mgr, filename, AASSET_MODE_UNKNOWN);
    int len = AAsset_getLength(pAsset);
    char *pBuffer = (char *) malloc(len + 1);
    pBuffer[len] = '\0';
    int numByte = AAsset_read(pAsset, pBuffer, len);
    XLOGE("readAssetFile", "numByte: %d, len: %d", numByte, len);
    AAsset_close(pAsset);
    return pBuffer;
}

void rgbToFrame(int width, int height, unsigned char *rgb, AVFrame *frame, int type) {
    const int frameSize = width * height;
    const int yuvType = (type & 0x10000) >> 16;
    const int byteRgba = (type & 0x0F000) >> 12;
    const int rShift = (type & 0x00F00) >> 8;
    const int gShift = (type & 0x000F0) >> 4;
    const int bShift = (type & 0x0000F);
    const int uIndex = 0;
    const int vIndex = yuvType; //yuvType为1表示YUV420p,为0表示420sp

    int yIndex = 0;
    int uvIndex[2] = {frameSize, frameSize + frameSize / 4};

    unsigned char R, G, B, Y, U, V;
    unsigned int index = 0;
    for (unsigned int j = 0; j < height; j++) {
        for (unsigned int i = 0; i < width; i++) {
            index = j * width + i;

            R = (rgb[index * byteRgba + rShift] & 0xFF);
            G = (rgb[index * byteRgba + gShift] & 0xFF);
            B = (rgb[index * byteRgba + bShift] & 0xFF);

            Y = (y(R, G, B));
            U = (u(R, G, B));
            V = (v(R, G, B));

            frame->data[0][j * frame->linesize[0] + i] = color(Y);
            if ((j & 1) == 0 && (index & 1) == 0) {
                frame->data[1][(j >> 1) * frame->linesize[1] + (i >> 1)] = color(U);
                frame->data[2][(j >> 1) * frame->linesize[2] + (i >> 1)] = color(V);
            }
        }
    }
}

void matrixSetIdentityM(float *m) {
    memset((void*)m, 0, 16*sizeof(float));
    m[0] = m[5] = m[10] = m[15] = 1.0f;
}

void matrixSetRotateM(float *m, float a, float x, float y, float z) {
    float s, c;

    memset((void*)m, 0, 15*sizeof(float));
    m[15] = 1.0f;

    a *= PI/180.0f;
    s = sin(a);
    c = cos(a);

    if (1.0f == x && 0.0f == y && 0.0f == z) {
        m[5] = c; m[10] = c;
        m[6] = s; m[9]  = -s;
        m[0] = 1;
    } else if (0.0f == x && 1.0f == y && 0.0f == z) {
        m[0] = c; m[10] = c;
        m[8] = s; m[2]  = -s;
        m[5] = 1;
    } else if (0.0f == x && 0.0f == y && 1.0f == z) {
        m[0] = c; m[5] = c;
        m[1] = s; m[4] = -s;
        m[10] = 1;
    } else {
        normalize(x, y, z);
        float nc = 1.0f - c;
        float xy = x * y;
        float yz = y * z;
        float zx = z * x;
        float xs = x * s;
        float ys = y * s;
        float zs = z * s;
        m[ 0] = x*x*nc +  c;
        m[ 4] =  xy*nc - zs;
        m[ 8] =  zx*nc + ys;
        m[ 1] =  xy*nc + zs;
        m[ 5] = y*y*nc +  c;
        m[ 9] =  yz*nc - xs;
        m[ 2] =  zx*nc - ys;
        m[ 6] =  yz*nc + xs;
        m[10] = z*z*nc +  c;
    }
}

void matrixMultiplyMM(float *m, float *lhs, float *rhs)
{
    float t[16];
    for (int i = 0; i < 4; i++) {
        const float rhs_i0 = rhs[I(i, 0)];
        float ri0 = lhs[ I(0,0) ] * rhs_i0;
        float ri1 = lhs[ I(0,1) ] * rhs_i0;
        float ri2 = lhs[ I(0,2) ] * rhs_i0;
        float ri3 = lhs[ I(0,3) ] * rhs_i0;
        for (int j = 1; j < 4; j++) {
            const float rhs_ij = rhs[ I(i,j) ];
            ri0 += lhs[ I(j,0) ] * rhs_ij;
            ri1 += lhs[ I(j,1) ] * rhs_ij;
            ri2 += lhs[ I(j,2) ] * rhs_ij;
            ri3 += lhs[ I(j,3) ] * rhs_ij;
        }
        t[ I(i,0) ] = ri0;
        t[ I(i,1) ] = ri1;
        t[ I(i,2) ] = ri2;
        t[ I(i,3) ] = ri3;
    }
    memcpy(m, t, sizeof(t));
}

void matrixTranslateM(float *m, float x, float y, float z)
{
    for (int i = 0; i < 4; i++)
    {
        m[12+i] += m[i]*x + m[4+i]*y + m[8+i]*z;
    }
}

void matrixRotateM(float *m, float a, float x, float y, float z)
{
    float rot[16], res[16];
    matrixSetRotateM(rot, a, x, y, z);
    matrixMultiplyMM(res, m, rot);
    memcpy(m, res, 16*sizeof(float));
}