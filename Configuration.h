//
//  Configuration.h
//  Workbench
//
//  Created by Wai Man Chan on 10/27/14.
//
//

#ifndef Workbench_Configuration_h
#define Workbench_Configuration_h

#define HomeKitLog 1

#define deviceName "Night Light"
#define deviceIdentity "12:00:54:23:51:13"
#define manufactuerName "ET Chan"
#define devicePassword "523-12-643"
#define deviceUUID "9FCF7180-6CAA-4174-ABC0-E3FAE58E3ADD"
#define controllerRecordsAddress "/Users/waimanchan/controller"

#include <CommonCrypto/CommonCrypto.h>

typedef CC_SHA512_CTX SHA_CTX;
#define SHAInit CC_SHA512_Init
#define SHAUpdate CC_SHA512_Update
#define SHAFinal CC_SHA512_Final
#define SHA_DIGESTSIZE 64

#endif
