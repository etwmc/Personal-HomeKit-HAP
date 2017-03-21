/*
 * This accessory.cpp is configurated for light accessory
 */

#include "Accessory.h"

#include "PHKAccessory.h"

#include "PHKNetworkIP.h"

#include <fstream>

#include <set>

using namespace std;

pthread_mutex_t recordMutex;

set <string> trackingUserList;
set <connectionInfo*> activeUsers;

intCharacteristics *occupyState;

#define userListAddr "./userList"

void _newConnection(connectionInfo* info) {
    printf("New connection %s\n", info->hostname.c_str());
    pthread_mutex_lock(&recordMutex);
    
    bool originalOutput = activeUsers.size() > 0;
    if ( trackingUserList.count(info->hostname) > 0 )
        activeUsers.insert(info);
    
    bool laterOutput = activeUsers.size() > 0;
    
    pthread_mutex_unlock(&recordMutex);
    if (originalOutput != laterOutput) {
        //Should notify
        printf("Changed\n");
        occupyState->notify();
    }
}

void _deadConnection(connectionInfo *info) {
    pthread_mutex_lock(&recordMutex);
    
    bool originalOutput = activeUsers.size() > 0;
    activeUsers.erase(info);
    
    bool laterOutput = activeUsers.size() > 0;
    
    pthread_mutex_unlock(&recordMutex);
    if (originalOutput != laterOutput) {
        //Should notify
        printf("Changed\n");
        occupyState->notify();
    }
}

void loadUserList() {
    ifstream fs;
    fs.open(userListAddr, std::ifstream::in);
    char buffer[256];
    bool isEmpty = fs.peek() == EOF;
    while (!isEmpty&&fs.is_open()&&fs.good()&&!fs.eof()) {
        fs.getline(buffer, 256);
        string s = string(buffer);
        trackingUserList.insert(s);
    }
    fs.close();
}

void saveUserList() {
    ofstream fs;
    fs.open(userListAddr, std::ifstream::out);
    for (set<string>::iterator it = trackingUserList.begin(); it != trackingUserList.end(); it++) {
        fs << *it << "\n";
    }
    fs.close();
}

string trackable(connectionInfo *sender) {
    pthread_mutex_lock(&recordMutex);
    string result = trackingUserList.count(sender->hostname) > 0? "1": "0";
    pthread_mutex_unlock(&recordMutex);
    return result;
}

string calculateOccupy(connectionInfo *sender) {
    pthread_mutex_lock(&recordMutex);
    string result = activeUsers.size() > 0? "1": "0";
    pthread_mutex_unlock(&recordMutex);
    return result;
}

void switchTrackable(bool oldValue, bool newValue, connectionInfo *sender) {
    if (newValue) {
        //Track this device
        trackingUserList.insert(sender->hostname);
        saveUserList();
        //Update active list
        _newConnection(sender);
    } else {
        //Stop tracking
        trackingUserList.erase(sender->hostname);
        saveUserList();
        //Update active list
        _deadConnection(sender);
    }
}

void identity(bool oldValue, bool newValue, connectionInfo *sender) {
    printf("Identify\n");
}

AccessorySet *accSet;

void initAccessorySet() {
    
    newConnection = &_newConnection;
    deadConnection = &_deadConnection;
    
    loadUserList();
    
    pthread_mutex_init(&recordMutex, NULL);
    
    currentDeviceType = deviceType_sensor;
    
    printf("Initial Sensor\n");
    accSet = &AccessorySet::getInstance();
    Accessory *sensorAcc = new Accessory();
    addInfoServiceToAccessory(sensorAcc, "Wi-Fi Sensor", "ET", "Wi-Fi Sensor v1", "12345678", &identity);
    accSet->addAccessory(sensorAcc);

    Service *sensorService = new Service(serviceType_occupancySensor);
    sensorAcc->addService(sensorService);

    stringCharacteristics *sensorServiceName = new stringCharacteristics(charType_serviceName, premission_read, 0);
    sensorServiceName->characteristics::setValue("Wi-Fi Sensor");
    sensorAcc->addCharacteristics(sensorService, sensorServiceName);

    boolCharacteristics *trackableState = new boolCharacteristics(0x10000, premission_read|premission_write);
    trackableState->characteristics::setValue("false");
    trackableState->perUserQuery = &trackable;
    trackableState->valueChangeFunctionCall = &switchTrackable;
    sensorAcc->addCharacteristics(sensorService, trackableState);

    occupyState = new intCharacteristics(charType_occupancyDetected, premission_read|premission_notify, 0, 1, 1, unit_none);
    occupyState->characteristics::setValue("0");
    occupyState->perUserQuery = &calculateOccupy;
    sensorAcc->addCharacteristics(sensorService, occupyState);
};
