#pragma once
//
//  PHKControllerRecord.h
//  Workbench
//
//  Created by Wai Man Chan on 9/23/14.
//
//

#include <stdio.h>
#include <vector>
using namespace std;

struct PHKKeyRecord {
    char controllerID[36];
    char publicKey[32];
};

void resetControllerRecord();
bool hasController();
void addControllerKey(PHKKeyRecord record);
bool doControllerKeyExist(PHKKeyRecord record);
void removeControllerKey(PHKKeyRecord record);
PHKKeyRecord getControllerKey(char key[32]);
