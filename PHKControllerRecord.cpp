//
//  PHKKeyRecord.cpp
//  Workbench
//
//  Created by Wai Man Chan on 9/23/14.
//
//

#include <vector>
#include <strings.h>

#if MCU
#else
#include <fstream>
#endif

using namespace std;


#include "PHKControllerRecord.h"
#include "Configuration.h"





void ControllerRecord::loadController() {
    ifstream fs;
#if MCU
#else
    fs.open(this->storeFilePath.c_str(), std::ifstream::in);
#endif

	char buffer[70] = {0};

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
	this->controllerRecords = results;
}

void ControllerRecord::resetController() {
	unlink(this->storeFilePath.c_str() );
	this->controllerRecords.clear();
}

bool ControllerRecord::hasController() {
    return this->controllerRecords.size() > 0;
}

void ControllerRecord::storeFiles()
{
#if MCU
#else
    ofstream fs;
    fs.open(this->storeFilePath.c_str(), std::ofstream::trunc);
#endif

    for (vector<PHKKeyRecord>::iterator it = this->controllerRecords.begin(); it != this->controllerRecords.end(); it++) {
#if MCU
#else
        fs.write(it->controllerID, 36);
        fs.write(it->publicKey, 32);
#endif
    }
    fs.close();
}


void ControllerRecord::addControllerKey(PHKKeyRecord record) {
    if (doControllerKeyExist(record) == false) {
        this->controllerRecords.push_back(record);
		storeFiles();
    }
}

bool ControllerRecord::doControllerKeyExist(PHKKeyRecord record) {
    for (vector<PHKKeyRecord>::iterator it = this->controllerRecords.begin(); it != this->controllerRecords.end(); it++) {
        if (bcmp((*it).controllerID, record.controllerID, 32) == 0) return true;
    }
    return false;
}

void ControllerRecord::removeControllerKey(PHKKeyRecord record) {
    for (vector<PHKKeyRecord>::iterator it = this->controllerRecords.begin(); it != this->controllerRecords.end(); it++) {
        if (bcmp((*it).controllerID, record.controllerID, 32) == 0) {
            this->controllerRecords.erase(it);
			storeFiles();
            return;
        }
    }
}

PHKKeyRecord ControllerRecord::getControllerKey(char key[32]) {
    for (vector<PHKKeyRecord>::iterator it = this->controllerRecords.begin(); it != this->controllerRecords.end(); it++) {
        if (bcmp(key, it->controllerID, 32) == 0) return *it;
    }
    PHKKeyRecord emptyRecord;
    bzero(emptyRecord.controllerID, 32);
    return emptyRecord;
}

