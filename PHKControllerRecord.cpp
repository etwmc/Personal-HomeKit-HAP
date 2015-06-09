//
//  PHKKeyRecord.cpp
//  Workbench
//
//  Created by Wai Man Chan on 9/23/14.
//
//

#include "PHKControllerRecord.h"
#include "Configuration.h"
#include <vector>
#include <strings.h>


#if MCU
#else
#include <fstream>
#endif

using namespace std;

vector<PHKKeyRecord>readIn();

vector<PHKKeyRecord>controllerRecords = readIn();

vector<PHKKeyRecord>readIn() {
    ifstream fs;
#if MCU
#else
    fs.open(controllerRecordsAddress, std::ifstream::in);
#endif

    char buffer[70];
    bzero(buffer, 70);

    PHKKeyRecord record;
    vector<PHKKeyRecord> results;

#if MCU
#else
    bool isEmpty = fs.peek() == EOF;
    while (!isEmpty&&fs.is_open()&&fs.good()&&!fs.eof()) {
        fs.get(buffer, 69);
#endif
        bcopy(buffer, record.controllerID, 36);
        bcopy(&buffer[36], record.publicKey, 32);
        results.push_back(record);
    }
#if MCU
#else
    fs.close();
#endif

    return results;
}

void resetControllerRecord() {
    ofstream fs;
    fs.open(controllerRecordsAddress, std::ofstream::out|std::ofstream::trunc);
}

bool hasController() {
    return controllerRecords.size() > 0;
}

void addControllerKey(PHKKeyRecord record) {
    if (doControllerKeyExist(record) == false) {
        controllerRecords.push_back(record);

#if MCU
#else
        ofstream fs;
        fs.open(controllerRecordsAddress, std::ofstream::trunc);
#endif

        for (vector<PHKKeyRecord>::iterator it = controllerRecords.begin(); it != controllerRecords.end(); it++) {
#if MCU
#else
            fs.write(it->controllerID, 36);
            fs.write(it->publicKey, 32);
#endif
        }
        fs.close();

    }
}

bool doControllerKeyExist(PHKKeyRecord record) {
    for (vector<PHKKeyRecord>::iterator it = controllerRecords.begin(); it != controllerRecords.end(); it++) {
        if (bcmp((*it).controllerID, record.controllerID, 32) == 0) return true;
    }
    return false;
}

void removeControllerKey(PHKKeyRecord record) {
    for (vector<PHKKeyRecord>::iterator it = controllerRecords.begin(); it != controllerRecords.end(); it++) {
        if (bcmp((*it).controllerID, record.controllerID, 32) == 0) {
            controllerRecords.push_back(record);
            return;
        }
    }
}

PHKKeyRecord getControllerKey(char key[32]) {
    for (vector<PHKKeyRecord>::iterator it = controllerRecords.begin(); it != controllerRecords.end(); it++) {
        if (bcmp(key, it->controllerID, 32) == 0) return *it;
    }
    PHKKeyRecord emptyRecord;
    bzero(emptyRecord.controllerID, 32);
    return emptyRecord;
}
