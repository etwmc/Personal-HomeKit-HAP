#pragma once
//
//  PHKNetworkIP.h
//  Workbench
//
//  Created by Wai Man Chan on 4/8/14.
//
//

#ifdef _WIN32
	#include <Ws2tcpip.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <time.h>
	#include <pthread.h>
#else
	#include <stdio.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
#endif

#include <dns_sd.h>  // linux->avahi  windows->bonjoursdk
#include <cstring>
#include <string>

#include "Configuration.h"

using namespace std;

#define IPv4 1
#define IPv6 0

typedef enum {
    deviceType_other = 0,
    deviceType_bridge = 2,
    deviceType_fan = 3,
    deviceType_garageDoorOpener = 4,
    deviceType_lightBulb = 5,
    deviceType_doorLock = 6,
    deviceType_outlet = 7,
    deviceType_switch = 8,
    deviceType_thermostat = 9,
    deviceType_sensor = 10,
    deviceType_alarmSystem = 11,
    deviceType_door = 12,
    deviceType_window = 13,
    deviceType_windowCover = 14,
    deviceType_programmableSwitch = 15,
} deviceType;

extern deviceType currentDeviceType;

typedef enum
{
    State_M1_SRPStartRequest      = 1,
    State_M2_SRPStartRespond      = 2,
    State_M3_SRPVerifyRequest     = 3,
    State_M4_SRPVerifyRespond     = 4,
    State_M5_ExchangeRequest      = 5,
    State_M6_ExchangeRespond      = 6,
} PairSetupState_t;

typedef enum
{
    State_Pair_Verify_M1          = 1,
    State_Pair_Verify_M2          = 2,
    State_Pair_Verify_M3          = 3,
    State_Pair_Verify_M4          = 4,
} PairVerifyState_t;

typedef enum
{
    Type_Data_Without_Length      = 1,
    Type_Data_With_Length         = 2,
} Poly1305Type_t;

void broadcastMessage(void *sender, char *resultData, size_t resultLen);

class PHKNetworkIP {

    void setupSocket();
    void handlePairSeup(int subSocket, char *buffer) const;
    void handlePairVerify(int subSocket, char *buffer) const;

public:
	PHKNetworkIP(const string& name
		,const string& pinCode
		,const string& identity
		,const string& controllerRecordsPath);
    void handleConnection() const;
    ~PHKNetworkIP();

    void closeAcceptConnection();
};

class PHKNetworkMessageDataRecord {
public:
    unsigned char index;
    char *data;
    unsigned int length;
    bool activate;
	PHKNetworkMessageDataRecord()
		: index(0)
		, data(0)
		, length(0)
		, activate(false)
	{
	}
    ~PHKNetworkMessageDataRecord();
    PHKNetworkMessageDataRecord &operator=(const PHKNetworkMessageDataRecord&);
};

class PHKNetworkMessageData {
    PHKNetworkMessageDataRecord records[10];
    unsigned char count;
public:
    PHKNetworkMessageData()
	{
		this->count = 0;
	}
    PHKNetworkMessageData(const char *rawData, unsigned short len);
    PHKNetworkMessageData(const PHKNetworkMessageData &data);
    PHKNetworkMessageData &operator=(const PHKNetworkMessageData &);
    void rawData(const char **dataPtr, unsigned short *len);
    void addRecord(PHKNetworkMessageDataRecord& record);
    int recordIndex(unsigned char index);
    char *dataPtrForIndex(unsigned char index);
    unsigned int lengthForIndex(unsigned char index);
};

class PHKNetworkMessage {
    char method[5];
    char type[40];
public:
    char directory[20];
    PHKNetworkMessageData data;
    PHKNetworkMessage(const char *rawData);
    void getBinaryPtr(char **buffer, int *contentLength);
};

class PHKNetworkResponse {
    unsigned short responseCode;
    string responseType();
public:
    PHKNetworkMessageData data;
    PHKNetworkResponse(unsigned short _responseCode);
    void getBinaryPtr(char **buffer, int *contentLength);
};


class connectionInfo {
public:
    pthread_t thread;
    pthread_mutex_t mutex;

    bool connected;

    uint8_t controllerToAccessoryKey[32];
    uint8_t accessoryToControllerKey[32];
    unsigned long long numberOfMsgRec;
    unsigned long long numberOfMsgSend;
    int subSocket;
    char buffer[4096+1];

    void *notificationList[numberOfNotifiableValue];

    void handlePairSeup();
    void handlePairVerify();
    void handleAccessoryRequest();

	connectionInfo()
	{
		init();
 	}
	void init()
	{
		subSocket = (-1);
		numberOfMsgRec = (0);
		numberOfMsgSend = (0);
		connected = (false);
		memset(buffer,0,4096+1);
		memset(notificationList,0,sizeof(void*)*numberOfNotifiableValue);
		memset(controllerToAccessoryKey,0,sizeof(uint8_t)*32);
		memset(accessoryToControllerKey,0,sizeof(uint8_t)*32);
	}

    void Poly1305_GenKey(const unsigned char * key, uint8_t * buf, uint16_t len, Poly1305Type_t type, char* verify);

    void addNotify(void *target) {
        for (int i = 0; i < numberOfNotifiableValue; i++) {
            if (notificationList[i] == 0) {
                notificationList[i] = target;
                printf("Add notify %d\n", target);
                return;
            }
        }
    }
    bool notify(void *target) {
        for (int i = 0; i < numberOfNotifiableValue; i++) {
            if (notificationList[i] == target) {
                return true;
            }
        }
        return false;
    }
    void removeNotify(void *target) {
        for (int i = 0; i < numberOfNotifiableValue; i++) {
            if (notificationList[i] == target)
                notificationList[i] = 0;
        }
    }
    void clearNotify() {
        for (int i = 0; i < numberOfNotifiableValue; i++) {
            notificationList[i] = 0;
        }
    }
};

void updateConfiguration();

//remove all controller pairing
void resetControllerAll();
