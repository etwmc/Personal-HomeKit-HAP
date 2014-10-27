
#define SHA_DIGESTSIZE 64

#include <CommonCrypto/CommonCrypto.h>

typedef CC_SHA512_CTX SHA_CTX;
#define SHAInit CC_SHA512_Init
#define SHAUpdate CC_SHA512_Update
#define SHAFinal CC_SHA512_Final
