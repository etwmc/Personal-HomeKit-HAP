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

class ControllerRecord
{
	string storeFilePath;
	vector<PHKKeyRecord> controllerRecords;

	void storeFiles();
	void loadController() ;
public:
	ControllerRecord() 
	{
	}
	virtual ~ControllerRecord()
	{
	}
	void create(const std::string& storeFilePath)
	{
		this->storeFilePath = storeFilePath;
		loadController();
	}

	void resetController() ;
	bool hasController() ;
	void addControllerKey(PHKKeyRecord record) ;
	bool doControllerKeyExist(PHKKeyRecord record) ;
	void removeControllerKey(PHKKeyRecord record) ;
	PHKKeyRecord getControllerKey(char key[32]) ;
};
