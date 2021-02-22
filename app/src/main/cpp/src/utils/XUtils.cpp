//
// Created by ArcherV on 2021-02-22.
//

#include <cstdlib>
#include "XUtils.h"
#include "Xlog.h"

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