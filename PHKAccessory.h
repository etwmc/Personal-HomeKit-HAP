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
    
#pragma - The following is only default by the device after iOS 9
    
    charType_firmwareRevision   = 0x52,
    charType_hardwareRevision   = 0x53,
    charType_softwareRevision   = 0x54,
    
    charType_reachable          = 0x63,
    
    charType_airParticulateDensity = 0x64,
    charType_airParticulateSize = 0x65,
    charType_airQuality         = 0x95,
    charType_carbonDioxideDetected = 0x92,
    charType_carbonMonoxideDetected = 0x69,
    charType_carbonDioxideLevel = 0x93,
    charType_carbonMonoxideLevel = 0x90,
    charType_carbonDioxidePeakLevel = 0x94,
    charType_carbonMonoxidePeakLevel = 0x91,
    charType_smokeDetected      = 0x76,
    
    charType_alarmCurrentState  = 0x66,
    charType_alarmTargetState   = 0x67,
    charType_batteryLevel       = 0x68,
    charType_contactSensorState = 0x6A,
    charType_holdPosition       = 0x6F,
    charType_leakDetected       = 0x70,
    charType_occupancyDetected  = 0x71,
    
    charType_currentAmbientLightLevel = 0x6B,
    charType_currentHorizontalTiltAngle = 0x6C,
    charType_targetHorizontalTiltAngle = 0x7B,
    charType_currentPosition    = 0x6D,
    charType_targetPosition     = 0x7C,
    charType_currentVerticalTiltAngle = 0x6E,
    charType_targetVerticalTiltAngle = 0x7D,
    
    charType_positionState      = 0x72,
    charType_programmableSwitchEvent = 0x73,
    charType_programmableSwitchOutputState = 0x74,
    
    charType_sensorActive       = 0x75,
    charType_sensorFault        = 0x77,
    charType_sensorJammed       = 0x78,
    charType_sensorLowBattery   = 0x79,
    charType_sensorTampered     = 0x7A,
    charType_sensorChargingState= 0x8F,
    
    
#pragma - The following is service provide
    serviceType_accessoryInfo      = 0x3E,
    serviceType_camera             = 0x3F,
    serviceType_fan                = 0x40,
    serviceType_garageDoorOpener   = 0x41,
    serviceType_lightBulb          = 0x43,
    serviceType_lockManagement     = 0x44,
    serviceType_lockMechanism      = 0x45,
    serviceType_microphone         = 0x46,
    serviceType_outlet             = 0x47,
    serviceType_speaker            = 0x48,
    serviceType_switch             = 0x49,
    serviceType_thermostat         = 0x4A,
    
    serviceType_alarmSystem        = 0x7E,
    serviceType_bridgingState      = 0x62,
    serviceType_carbonMonoxideSensor = 0x7F,
    serviceType_contactSensor      = 0x80,
    serviceType_door               = 0x81,
    serviceType_humiditySensor     = 0x82,
    serviceType_leakSensor         = 0x83,
    serviceType_lightSensor        = 0x84,
    serviceType_motionSensor       = 0x85,
    serviceType_occupancySensor    = 0x86,
    serviceType_smokeSensor        = 0x87,
    serviceType_programmableSwitch_stateful = 0x88,
    serviceType_programmableSwitch_stateless = 0x89,
    serviceType_temperatureSensor  = 0x8A,
    serviceType_window             = 0x8B,
    serviceType_windowCover        = 0x8C,
    serviceType_airQualitySensor   = 0x8C,
    serviceType_securityAlarm      = 0x8E,
    serviceType_charging           = 0x8F,
    
    serviceType_battery            = 0x96,
    serviceType_carbonDioxideSensor= 0x97,
    
#pragma - The following is for bluetooth characteristic
    btCharType_pairSetup = 0x4C,
    btCharType_pairVerify = 0x4E,
    btCharType_pairingFeature = 0x4F,
    btCharType_pairings = 0x50,
    btCharType_serviceInstanceID = 0x51,
#pragma - The following is for bluetooth service
    btServiceType_accessoryInformation = 0xFED3,
    btServiceType_camera            = 0xFEC9,
    btServiceType_fan               = 0xFECB,
    btServiceType_garageDoorOpener  = 0xFECE,
    btServiceType_lightBulb         = 0xFED2,
    btServiceType_lockManagement    = 0xFECF,
    btServiceType_lockMechanism     = 0xFED0,
    btServiceType_microphone        = 0xFEC8,
    btServiceType_outlet            = 0xFECC,
    btServiceType_speaker           = 0xFEC7,
    btServiceType_switch            = 0xFECD,
    
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

class Accessory;

class characteristics {
public:
    Accessory *accessory;
    const unsigned int type;
    const int premission;
    int iid;
    characteristics(unsigned int _type, int _premission): type(_type), premission(_premission) {}
    virtual string value(connectionInfo *sender) = 0;
    void setValue(string str) {
        setValue(str, NULL);
    }
    virtual void setValue(string str, connectionInfo *sender) = 0;
    virtual string describe(connectionInfo *sender) = 0;
    string (*perUserQuery)(connectionInfo *sender) = 0;
    bool writable() { return premission&premission_write; }
    bool notifiable() { return premission&premission_notify; }
    void notify();
};

//To store value of device state, subclass the following type
class boolCharacteristics: public characteristics {
public:
    bool _value;
    void (*valueChangeFunctionCall)(bool oldValue, bool newValue, connectionInfo *sender) = NULL;
    boolCharacteristics(unsigned int _type, int _premission): characteristics(_type, _premission) {}
    virtual string value(connectionInfo *sender) {
        if (perUserQuery != 0)
            return perUserQuery(sender);
        if (_value)
            return "1";
        return "0";
    }
    virtual void setValue(string str, connectionInfo *sender) {
        bool newValue = (strncmp("true", str.c_str(), 4)==0)||(strncmp("1", str.c_str(), 1)==0);
        if (valueChangeFunctionCall)
            valueChangeFunctionCall(_value, newValue, sender);
        _value = newValue;
    }
    virtual string describe(connectionInfo *sender);
};

class floatCharacteristics: public characteristics {
public:
    float _value;
    const float _minVal, _maxVal, _step;
    const unit _unit;
    void (*valueChangeFunctionCall)(float oldValue, float newValue, connectionInfo *sender) = NULL;
    floatCharacteristics(unsigned int _type, int _premission, float minVal, float maxVal, float step, unit charUnit): characteristics(_type, _premission), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit) {}
    virtual string value(connectionInfo *sender) {
        if (perUserQuery != 0)
            return perUserQuery(sender);
        char temp[16];
        snprintf(temp, 16, "%f", _value);
        return temp;
    }
    virtual void setValue(string str, connectionInfo *sender) {
        float temp = atof(str.c_str());
        if (temp == temp) {
            if (valueChangeFunctionCall)
                valueChangeFunctionCall(_value, temp, sender);
            _value = temp;
        }
    }
    virtual string describe(connectionInfo *sender);
};

class intCharacteristics: public characteristics {
public:
    int _value;
    const int _minVal, _maxVal, _step;
    const unit _unit;
    void (*valueChangeFunctionCall)(int oldValue, int newValue, connectionInfo *sender) = NULL;
    intCharacteristics(unsigned int _type, int _premission, int minVal, int maxVal, int step, unit charUnit): characteristics(_type, _premission), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit) {
        _value = minVal;
    }
    virtual string value(connectionInfo *sender) {
        if (perUserQuery != 0)
            return perUserQuery(sender);
        char temp[16];
        snprintf(temp, 16, "%d", _value);
        return temp;
    }
    virtual void setValue(string str, connectionInfo *sender) {
        float temp = atoi(str.c_str());
        if (temp == temp) {
            if (valueChangeFunctionCall)
                valueChangeFunctionCall(_value, temp, sender);
            _value = temp;
        }
    }
    virtual string describe(connectionInfo *sender);
};

class stringCharacteristics: public characteristics {
public:
    string _value;
    const unsigned short maxLen;
    void (*valueChangeFunctionCall)(string oldValue, string newValue, connectionInfo *sender) = NULL;
    stringCharacteristics(unsigned int _type, int _premission, unsigned short _maxLen): characteristics(_type, _premission), maxLen(_maxLen) {}
    virtual string value(connectionInfo *sender) {
        if (perUserQuery != 0)
            return "\""+perUserQuery(sender)+"\"";
        return "\""+_value+"\"";
    }
    virtual void setValue(string str, connectionInfo *sender) {
        if (valueChangeFunctionCall)
            valueChangeFunctionCall(_value, str, sender);
        _value = str;
    }
    virtual string describe(connectionInfo *sender);
};

//Abstract Layer of object
class Service {
public:
    int serviceID, uuid;
    vector<characteristics *> _characteristics;
    Service(int _uuid): uuid(_uuid) {}
    virtual short numberOfCharacteristics() { return _characteristics.size(); }
    virtual characteristics *characteristicsAtIndex(int index) { return _characteristics[index]; }
    string describe(connectionInfo *sender);
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
        cha->accessory = this;
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
    string describe(connectionInfo *sender);
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
    string describe(connectionInfo *sender);
};

typedef void (*identifyFunction)(bool oldValue, bool newValue, connectionInfo *sender);

//Since Info Service contains only constant, only add method will be provided
void addInfoServiceToAccessory(Accessory *acc, string accName, string manufactuerName, string modelName, string serialNumber, identifyFunction identifyCallback);

void handleAccessory(const char *request, unsigned int requestLen, char **reply, unsigned int *replyLen, connectionInfo *sender);

void updateValueFromDeviceEnd(characteristics *c, int aid, int iid, string value);

struct broadcastInfo {
    void *sender;
    char *desc;
};
void *announce(void *info);
