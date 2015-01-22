#pragma once
//
//  PHKAccessory.h
//  Workbench
//
//  Created by Wai Man Chan on 9/27/14.
//
//

#include <string.h>
#include <strings.h>
#define __STDC_LIMIT_MACROS
#include <stdint.h>
extern "C" {
#include <stdlib.h>
}

#include "PHKNetworkIP.h"

#include "PHKControllerRecord.h"

extern "C" {
#include "PHKArduinoLightInterface.h"
}

#include <vector>

#if MCU
#else
#include <pthread.h>
#endif

using namespace std;

typedef enum {
    charType_adminOnlyAccess    = 0x1,
    charType_audioChannels      = 0x2,
    charType_audioCodexName     = 0x3,
    charType_audioCodexParameter= 0x4,
    charType_audioFeedback      = 0x5,
    charType_audioPropAttr      = 0x6,
    charType_audioValAttr       = 0x7,
    charType_brightness         = 0x8,
    charType_cameraNightVision  = 0x9,
    charType_cameraPan          = 0xA,
    charType_cameraTilt         = 0xB,
    charType_cameraZoom         = 0xC,
    charType_coolingThreshold   = 0xD,
    charType_currentDoorState   = 0xE,
    charType_currentHeatCoolMode= 0xF,
    charType_currentHumidity    = 0x10,
    charType_currentTemperature = 0x11,
    charType_heatingThreshold   = 0x12,
    charType_hue                = 0x13,
    charType_identify           = 0x14,
    charType_inputVolume        = 0x15,
    charType_ipCameraStart      = 0x16,
    charType_ipCameraStop       = 0x17,
    charType_lockControlPoint   = 0x19,
    charType_lockAutoTimeout    = 0x1A,
    charType_lockLastAction     = 0x1C,
    charType_lockCurrentState   = 0x1D,
    charType_lockTargetState    = 0x1E,
    charType_logs               = 0x1F,
    charType_manufactuer        = 0x20,
    charType_modelName          = 0x21,
    charType_motionDetect       = 0x22,
    charType_serviceName        = 0x23,
    charType_obstruction        = 0x24,
    charType_on                 = 0x25,
    charType_outletInUse        = 0x26,
    charType_outputVolume       = 0x27,
    charType_rotationDirection  = 0x28,
    charType_rotationSpeed      = 0x29,
    charType_rtcpExtProp        = 0x2A,
    charType_rtcpVideoPayload   = 0x2B,
    charType_rtcpAudioPayload   = 0x2C,
    charType_rtcpAudioClock     = 0x2D,
    charType_rtcpProtocol       = 0x2E,
    charType_saturation         = 0x2F,
    charType_serialNumber       = 0x30,
    charType_srtpCyptoSuite     = 0x31,
    charType_targetDoorState    = 0x32,
    charType_targetHeatCoolMode = 0x33,
    charType_targetHumidity     = 0x34,
    charType_targetTemperature  = 0x35,
    charType_temperatureUnit    = 0x36,
    charType_version            = 0x37,
    charType_videoCodexName     = 0x38,
    charType_videoCodexPara     = 0x39,
    charType_videoMirror        = 0x3A,
    charType_videoPropAttr      = 0x3B,
    charType_videoRotation      = 0x3C,
    charType_videoValAttr       = 0x3D,

#pragma - The following is service provide
    charType_accessoryInfo      = 0x3E,
    charType_camera             = 0x3F,
    charType_fan                = 0x40,
    charType_garageDoorOpener   = 0x41,
    charType_lightBulb          = 0x43,
    charType_lockManagement     = 0x44,
    charType_lockMechanism      = 0x45,
    charType_microphone         = 0x46,
    charType_outlet             = 0x47,
    charType_speaker            = 0x48,
    charType_switch             = 0x49,
    charType_thermostat         = 0x4A
} charType;

enum {
    premission_read = 1,
    premission_write = 1 << 1,
    premission_notify = 1 << 2  //Notify = Accessory will notice the controller
};

typedef enum {
    unit_none = 0,
    unit_celsius,
    unit_percentage,
    unit_arcDegree
} unit;


class characteristics {
public:

    const unsigned short type;
    const int premission;
    int iid;
    characteristics(unsigned short _type, int _premission): type(_type), premission(_premission) {}
    virtual string value() = 0;
    virtual void setValue(string str) = 0;
    virtual string describe() = 0;
    bool writable() { return premission&premission_write; }
    bool notifiable() { return premission&premission_notify; }
};

//To store value of device state, subclass the following type
class boolCharacteristics: public characteristics {
public:
    bool _value;
    void (*valueChangeFunctionCall)(bool oldValue, bool newValue) = NULL;
    boolCharacteristics(unsigned short _type, int _premission): characteristics(_type, _premission) {}
    virtual string value() {
        if (_value)
            return "1";
        return "0";
    }
    virtual void setValue(string str) {
        bool newValue = (strncmp("true", str.c_str(), 4)==0);
        if (valueChangeFunctionCall)
            valueChangeFunctionCall(_value, newValue);
        _value = newValue;
    }
    virtual string describe();
};

class floatCharacteristics: public characteristics {
public:
    float _value;
    const float _minVal, _maxVal, _step;
    const unit _unit;
    void (*valueChangeFunctionCall)(float oldValue, float newValue) = NULL;
    floatCharacteristics(unsigned short _type, int _premission, float minVal, float maxVal, float step, unit charUnit): characteristics(_type, _premission), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit) {}
    virtual string value() {
        char temp[16];
        snprintf(temp, 16, "%f", _value);
        return temp;
    }
    virtual void setValue(string str) {
        float temp = atof(str.c_str());
        if (temp == temp) {
            if (valueChangeFunctionCall)
                valueChangeFunctionCall(_value, temp);
            _value = temp;
        }
    }
    virtual string describe();
};

class intCharacteristics: public characteristics {
public:
    int _value;
    const int _minVal, _maxVal, _step;
    const unit _unit;
    void (*valueChangeFunctionCall)(int oldValue, int newValue) = NULL;
    intCharacteristics(unsigned short _type, int _premission, int minVal, int maxVal, int step, unit charUnit): characteristics(_type, _premission), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit) {
        _value = minVal;
    }
    virtual string value() {
        char temp[16];
        snprintf(temp, 16, "%d", _value);
        return temp;
    }
    virtual void setValue(string str) {
        float temp = atoi(str.c_str());
        if (temp == temp) {
            if (valueChangeFunctionCall)
                valueChangeFunctionCall(_value, temp);
            _value = temp;
        }
    }
    virtual string describe();
};

class stringCharacteristics: public characteristics {
public:
    string _value;
    const unsigned short maxLen;
    void (*valueChangeFunctionCall)(string oldValue, string newValue) = NULL;
    stringCharacteristics(unsigned short _type, int _premission, unsigned short _maxLen): characteristics(_type, _premission), maxLen(_maxLen) {}
    virtual string value() {
        return "\""+_value+"\"";
    }
    virtual void setValue(string str) {
        if (valueChangeFunctionCall)
            valueChangeFunctionCall(_value, str);
        _value = str;
    }
    virtual string describe();
};

//Abstract Layer of object
class Service {
public:
    int serviceID, uuid;
    vector<characteristics *> _characteristics;
    Service(int _uuid): uuid(_uuid) {}
    virtual short numberOfCharacteristics() { return _characteristics.size(); }
    virtual characteristics *characteristicsAtIndex(int index) { return _characteristics[index]; }
    string describe();
};

class Accessory {
public:
    int numberOfInstance = 0;
    int aid;
    vector<Service *>_services;
    void addService(Service *ser) {
        ser->serviceID = ++numberOfInstance;
        _services.push_back(ser);
    }
    void addCharacteristics(Service *ser, characteristics *cha) {
        cha->iid = ++numberOfInstance;
        ser->_characteristics.push_back(cha);
    }
    bool removeService(Service *ser) {
        bool exist = false;
        for (vector<Service *>::iterator it = _services.begin(); it != _services.end(); it++) {
            if (*it == ser) {
                _services.erase(it);
                exist = true;
            }
        }
        return exist;
    }
    bool removeCharacteristics(characteristics *cha) {
        bool exist = false;
        for (vector<Service *>::iterator it = _services.begin(); it != _services.end(); it++) {
            for (vector<characteristics *>::iterator jt = (*it)->_characteristics.begin(); jt != (*it)->_characteristics.end(); jt++) {
                if (*jt == cha) {
                    (*it)->_characteristics.erase(jt);
                    exist = true;
                }
            }
        }
        return exist;
    }
    Accessory() {}
    short numberOfService() { return _services.size(); }
    Service *serviceAtIndex(int index) {
        for (vector<Service *>::iterator it = _services.begin(); it != _services.end(); it++) {
            if ((*it)->serviceID == index) {
                return *it;
            }
        }
        return NULL;
    }
    characteristics *characteristicsAtIndex(int index) {
        for (vector<Service *>::iterator it = _services.begin(); it != _services.end(); it++) {
            for (vector<characteristics *>::iterator jt = (*it)->_characteristics.begin(); jt != (*it)->_characteristics.end(); jt++) {
                if ((*jt)->iid == index) {
                    return *jt;
                }
            }
        }
        return NULL;
    }
    string describe();
};

class AccessorySet {
private:
    vector<Accessory *> _accessories;
    int _aid = 0;
    AccessorySet() {
        pthread_mutex_init(&accessoryMutex, NULL);
    }
    AccessorySet(AccessorySet const&);
    void operator=(AccessorySet const&);
public:
    static AccessorySet& getInstance() {
        static AccessorySet instance;

        return instance;
    }
    pthread_mutex_t accessoryMutex;
    short numberOfAccessory() {
        return _accessories.size();
    }
    Accessory *accessoryAtIndex(int index) {
        for (vector<Accessory *>::iterator it = _accessories.begin(); it != _accessories.end(); it++) {
            if ((*it)->aid == index) {
                return *it;
            }
        }
        return NULL;
    }
    void addAccessory(Accessory *acc) {
        acc->aid = ++_aid;
        _accessories.push_back(acc);
    }
    bool removeAccessory(Accessory *acc) {
        bool exist = false;
        for (vector<Accessory *>::iterator it = _accessories.begin(); it != _accessories.end(); it++) {
            if (*it == acc) {
                _accessories.erase(it);
                exist = true;
            }
        }
        return exist;
    }
    ~AccessorySet() {
        pthread_mutex_destroy(&accessoryMutex);
    }
    string describe();
};

typedef void (*identifyFunction)(bool oldValue, bool newValue);

//Since Info Service contains only constant, only add method will be provided
void addInfoServiceToAccessory(Accessory *acc, string accName, string manufactuerName, string modelName, string serialNumber, identifyFunction identifyCallback);

void handleAccessory(const char *request, unsigned int requestLen, char **reply, unsigned int *replyLen, connectionInfo *sender);
