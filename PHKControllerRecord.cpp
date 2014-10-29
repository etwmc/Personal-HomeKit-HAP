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

#ifdef MCU
#else
#include <fstream>
#endif

using namespace std;

vector<PHKKeyRecord>readIn();

vector<PHKKeyRecord>controllerRecords = readIn();

vector<PHKKeyRecord>readIn() {
    ifstream fs;
    fs.open(controllerRecordsAddress, std::ifstream::in);
    
    char buffer[69];
    
    PHKKeyRecord record;
    vector<PHKKeyRecord> results;
    
    while (fs.is_open()&&fs.good()&&!fs.eof()) {
        fs.get(buffer, 69);
        bcopy(buffer, record.controllerID, 36);
        bcopy(&buffer[36], record.publicKey, 32);
        results.push_back(record);
    }
    fs.close();
    
    return results;
}

void addControllerKey(PHKKeyRecord record) {
    if (doControllerKeyExist(record) == false) {
        controllerRecords.push_back(record);
        
        ofstream fs;
        fs.open(controllerRecordsAddress, std::ofstream::trunc);
        
        for (auto it = controllerRecords.begin(); it != controllerRecords.end(); it++) {
            fs.write(it->controllerID, 36);
            fs.write(it->publicKey, 32);
        }
        fs.close();
        
    }
}

bool doControllerKeyExist(PHKKeyRecord record) {
    for (auto it = controllerRecords.begin(); it != controllerRecords.end(); it++) {
        if (bcmp((*it).controllerID, record.controllerID, 32) == 0) return true;
    }
    return false;
}

void removeControllerKey(PHKKeyRecord record) {
    for (auto it = controllerRecords.begin(); it != controllerRecords.end(); it++) {
        if (bcmp((*it).controllerID, record.controllerID, 32) == 0) {
            controllerRecords.push_back(record);
            return;
        }
    }
}

PHKKeyRecord getControllerKey(char key[32]) {
    for (auto it = controllerRecords.begin(); it != controllerRecords.end(); it++) {
        if (bcmp(key, it->controllerID, 32) == 0) return *it;
    }
    PHKKeyRecord emptyRecord;
    bzero(emptyRecord.controllerID, 32);
    return emptyRecord;
}
