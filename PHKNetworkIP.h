//
//  PHKNetworkIP.h
//  Workbench
//
//  Created by Wai Man Chan on 4/8/14.
//
//

#ifndef __Workbench__PHKNetworkIP__
#define __Workbench__PHKNetworkIP__

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dns_sd.h>
#include <cstring>
#include <string>

#include "Configuration.h"

using namespace std;

#define IPv4 1
#define IPv6 0

void broadcastMessage(void *sender, char *resultData, size_t resultLen);

class PHKNetworkIP {
    void setupSocket();
    void handlePairSeup(int subSocket, char *buffer) const;
    void handlePairVerify(int subSocket, char *buffer) const;
public:
    PHKNetworkIP();
    void handleConnection() const;
    ~PHKNetworkIP();
};

class PHKNetworkMessageDataRecord {
public:
    unsigned char index = 0;
    char *data = 0;
    unsigned int length = 0;
    bool activate = false;
    ~PHKNetworkMessageDataRecord();
    PHKNetworkMessageDataRecord &operator=(const PHKNetworkMessageDataRecord&);
};

class PHKNetworkMessageData {
    PHKNetworkMessageDataRecord records[10];
    unsigned char count = 0;
public:
    PHKNetworkMessageData() {}
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
    
    bool connected = false;
    
    uint8_t controllerToAccessoryKey[32];
    uint8_t accessoryToControllerKey[32];
    unsigned long long numberOfMsgRec = 0;
    unsigned long long numberOfMsgSend = 0;
    int subSocket = -1;
    char buffer[4096];
    
    void *notificationList[numberOfNotifiableValue];
    
    void handlePairSeup();
    void handlePairVerify();
    
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

#endif /* defined(__Workbench__PHKNetworkIP__) */
