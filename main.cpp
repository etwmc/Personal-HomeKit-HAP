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
}

using namespace std;

int main(int argc, const char * argv[]) {
    // insert code here...
    if (argc > 1) {
	//If there's some argument
	//Currently means reset
	resetControllerRecord();
    }

    initAccessorySet();
    setupPort();

    PHKNetworkIP networkIP;
    do {
        networkIP.handleConnection();
    } while (true);
    return 0;
}
