//
//  sha.c
//  Workbench
//
//  Created by Wai Man Chan on 10/31/14.
//
//

#include "sha.h"

int SHA512Reset(SHACTX *c) {
    SHAInit(c);
    return 0;
}
int SHA512Input(SHACTX *c, const void *data, unsigned int len) {
    SHAUpdate(c, data, len);
    return 0;
}
int SHA512FinalBits(SHACTX *c, const void *data, unsigned int len) {
    SHAUpdate(c, data, len);
    return 0;
}
int SHA512Result(SHACTX *c, unsigned char *md) {
    SHAFinal(md, c);
    return 0;
}