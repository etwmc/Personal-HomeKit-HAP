//
//  PHKControllerRecord.h
//  Workbench
//
//  Created by Wai Man Chan on 9/23/14.
//
//

#ifndef __Workbench__PHKControllerRecord__
#define __Workbench__PHKControllerRecord__

#include <stdio.h>
#include <vector>
using namespace std;

struct PHKKeyRecord {
    char controllerID[36];
    char publicKey[32];
};

void addControllerKey(PHKKeyRecord record);
bool doControllerKeyExist(PHKKeyRecord record);
void removeControllerKey(PHKKeyRecord record);
PHKKeyRecord getControllerKey(char key[32]);

#endif /* defined(__Workbench__PHKControllerRecord__) */
