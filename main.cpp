//
//  main.cpp
//  Personal_HomeKit
//
//  Created by Wai Man Chan on 4/8/14.
//
//

#include <iostream>
#include <fstream>
#include "PHKNetworkIP.h"
#include "Accessory.h"
#include "PHKControllerRecord.h"
extern "C" {
    #include "PHKArduinoLightInterface.h"
    #include "poly1305-opt-master/poly1305.h"
}

using namespace std;

#if PowerOnTest==1
bool poly1305_verify(const unsigned char *expected, const unsigned char *mac) {
    for (int i = 0; i < 16; i++) {
        if (expected[i] != mac[i])
            return false;
    }
    return true;
}

void
poly1305_auth(unsigned char mac[16], const unsigned char *m, size_t bytes, const unsigned char key[32]) {
    poly1305_context ctx;
    poly1305_init(&ctx, key);
    poly1305_update(&ctx, m, bytes);
    poly1305_finish(&ctx, mac);
}

int poly1305_power_on_self_test() {
    const unsigned char expected[16] = {0xdd,0xb9,0xda,0x7d,0xdd,0x5e,0x52,0x79,0x27,0x30,0xed,0x5c,0xda,0x5f,0x90,0xa4};
    unsigned char key[32];
    unsigned char mac[16];
    unsigned char msg[73];
    size_t i;
    
    for (i = 0; i < sizeof(key); i++)
        key[i] = (unsigned char)(i + 221);
    for (i = 0; i < sizeof(msg); i++)
        msg[i] = (unsigned char)(i + 121);
    poly1305_auth(mac, msg, sizeof(msg), key);
    
    printf("sample mac is ");
    for (i = 0; i < sizeof(mac); i++)
        printf("%02x", mac[i]);
    printf(" (%s)\n", poly1305_verify(expected, mac) ? "correct" : "incorrect");
}
#endif

int main(int argc, const char * argv[]) {
    
#if PowerOnTest==1
    printf("poweron: %d\n", poly1305_power_on_self_test());
#endif
    
    // insert code here...
    if (argc > 1) {
	//If there's some argument
	//Currently means reset
	resetControllerAll();
    }

    initAccessorySet();
#ifdef _WIN32
#else
	setupPort();
#endif

	PHKNetworkIP networkIP(deviceName,devicePassword,deviceIdentity,controllerRecordsAddress);
    do {
        networkIP.handleConnection();
    } while (true);

	//if you running PHKNetworkIP new accept thread, 
	//call PHKNetworkIP::closeAcceptConnection to stop.

#ifdef _WIN32
	WSACleanup();
#endif    
	return 0;
}
