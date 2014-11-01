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
#define HomeKitReplyHeaderLog 1

//Device Setting
#define deviceName "Night Light"    //Name
#define deviceIdentity "12:00:54:23:51:13"  //ID
#define manufactuerName "ET Chan"   //Manufactuer
#define devicePassword "523-12-643" //Password
#define deviceUUID "9FCF7180-6CAA-4174-ABC0-E3FAE58E3ADD"   //UUID, for pair verify
#define controllerRecordsAddress  //Where to store the client keys

//Number of client
#define numberOfClient 20

//If you compiling this to microcontroller, set it to 1
#define MCU 0

#include <openssl/sha.h>
#include <stdint.h>
#include <unistd.h>

typedef SHA512_CTX SHACTX;
#define SHAInit SHA512_Init
#define SHAUpdate SHA512_Update
#define SHAFinal SHA512_Final
#define SHA_DIGESTSIZE 64
#define SHA_BlockSize 128

#endif
