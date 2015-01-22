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
extern "C" {
#include "PHKArduinoLightInterface.h"
}

using namespace std;

int main(int argc, const char * argv[]) {
    // insert code here...
    initAccessorySet();
    setupPort();

    PHKNetworkIP networkIP;
    do {
        networkIP.handleConnection();
    } while (true);
    return 0;
}
