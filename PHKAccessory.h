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

#include "Configuration.h"

extern "C" {
#include "PHKArduinoLightInterface.h"
}


using namespace std;

typedef enum {
    charType_adminOnlyAccess    = 0x1,
    charType_audioFeedback      = 0x5,
    charType_brightness         = 0x8,
    charType_coolingThreshold   = 0xD,
    charType_currentDoorState   = 0xE,
    charType_currentHeatCoolMode= 0xF,
    charType_currentHumidity    = 0x10,
    charType_currentTemperature = 0x11,
    charType_heatingThreshold   = 0x12,
    charType_hue                = 0x13,
    charType_identify           = 0x14,
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
    charType_outletUse          = 0x26,
    charType_saturation         = 0x2F,
    charType_serialNumber       = 0x30,
    charType_targetDoorState    = 0x32,
    charType_targetHeatCoolMode = 0x33,
    charType_targetHumidity     = 0x34,
    charType_targetTemperature  = 0x35,
    charType_temperatureUnit    = 0x36,
    charType_version            = 0x37,
#pragma - The following is service provide
    charType_accessoryInfo      = 0x3E,
    charType_fan                = 0x3F,
    charType_garageDoorOpener   = 0x41,
    charType_lightBulb          = 0x43,
    charType_lockMechanism      = 0x45,
    charType_outlet             = 0x47,
    charType_switch             = 0x49,
    charType_thermostat         = 0x4A
} charType;

enum {
    premission_read = 1,
    premission_write = 1 << 1,
    premission_update = 1 << 2  //Update = Accessory will notice the controller
};

typedef enum {
    unit_none = 0,
    unit_celsius,
    unit_percentage,
    unit_arcDegree
} unit;

class characteristics {
protected:
    const int iid;
    const unsigned short type;
    const int premission;
public:
    characteristics(int _iid, unsigned short _type, int _premission): iid(_iid), type(_type), premission(_premission) {}
    virtual string value() = 0;
    virtual void setValue(string str) = 0;
    virtual string describe() = 0;
    bool writable() { return premission&premission_write; }
    bool update() { return premission&premission_update; }
};

//To store value of device state, subclass the following type
class boolCharacteristics: public characteristics {
protected:
    bool _value;
public:
    boolCharacteristics(int _iid, unsigned short _type, int _premission): characteristics(_iid, _type, _premission) {}
    virtual string value() {
        if (_value)
            return "1";
        return "0";
    }
    virtual void setValue(string str) {
        _value = (strncmp("true", str.c_str(), 4)==0);
    }
    virtual string describe();
};

class floatCharacteristics: public characteristics {
protected:
    float _value;
    const float _minVal, _maxVal, _step;
    const unit _unit;
public:
    floatCharacteristics(int _iid, unsigned short _type, int _premission, float minVal, float maxVal, float step, unit charUnit): characteristics(_iid, _type, _premission), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit) {}
    virtual string value() {
        char temp[16];
        snprintf(temp, 16, "%f", _value);
        return temp;
    }
    virtual void setValue(string str) {
        float temp = atof(str.c_str());
        if (temp == temp) {
            _value = temp;
        }
    }
    virtual string describe();
};

class intCharacteristics: public characteristics {
protected:
    int _value;
    const int _minVal, _maxVal, _step;
    const unit _unit;
public:
    intCharacteristics(int _iid, unsigned short _type, int _premission, int minVal, int maxVal, int step, unit charUnit): characteristics(_iid, _type, _premission), _minVal(minVal), _maxVal(maxVal), _step(step), _unit(charUnit) {}
    virtual string value() {
        char temp[16];
        snprintf(temp, 16, "%d", _value);
        return temp;
    }
    virtual void setValue(string str) {
        float temp = atoi(str.c_str());
        if (temp == temp) {
            _value = temp;
        }
    }
    virtual string describe();
};

class stringCharacteristics: public characteristics {
protected:
    string _value;
    const unsigned short maxLen;
public:
    stringCharacteristics(int _iid, unsigned short _type, int _premission, unsigned short _maxLen): characteristics(_iid, _type, _premission), maxLen(_maxLen) {}
    virtual string value() {
        return _value;
    }
    virtual void setValue(string str) {
        _value = str;
    }
    virtual string describe();
};

//To identify the accessory, finish the function identify() with the method
//And situation with multiple accessory, renew
class identifyCharacteristics: public boolCharacteristics {
public:
    identifyCharacteristics(int iid): boolCharacteristics(iid, charType_identify, premission_write) {
        startIdentify();
    }
    void setValue(string str) {
        boolCharacteristics::setValue(str);
        if (_value)
            identify();
    }
    virtual void identify() {
        startIdentify();
    }
    virtual string describe();
};

//Abstract Layer of object
class Service {
public:
    const int serviceID, uuid;
    Service(int _serviceID, int _uuid): serviceID(_serviceID), uuid(_uuid) {}
    virtual short numberOfCharacteristics() { return 0; }
    virtual characteristics *characteristicsAtIndex(int index) { return 0; }
    string describe();
};

class Accessory {
public:
    const int aid;
    Accessory(int _aid): aid(_aid) {}
    virtual short numberOfService() { return 0; }
    virtual Service *serviceAtIndex(int index) {
        return 0;
    }
    characteristics *characteristicsAtIndex(int index) {
        unsigned short no = numberOfService();
        for (int i = 1; i <= no; i++) {
            Service *s1 = serviceAtIndex(i+1);
            if (s1) {
                if (index < s1->serviceID) {
                    return serviceAtIndex(i)->characteristicsAtIndex(index);
                }
            }
            else
                return serviceAtIndex(i)->characteristicsAtIndex(index);
        }
        return 0;
    }
    string describe();
};

class AccessorySet {
    Accessory *mainAccessory;
public:
    virtual short numberOfAccessory() {
        return 0;
    }
    virtual Accessory *accessoryAtIndex(int index) {
        return 0;
    }
    string describe();
};

//For all the service type, subclass from Service or its subclass
//Instance ID must be bigger than 0
class infoService: public Service {
    stringCharacteristics name;
    stringCharacteristics manufactuer;
    stringCharacteristics modelName;
    stringCharacteristics serialNumber;
    identifyCharacteristics identify;
public:
    infoService(int index): Service(index, charType_accessoryInfo),
    name(index+1, charType_serviceName, premission_read, 0),
    manufactuer(index+2, charType_manufactuer, premission_read, 0),
    modelName(index+3, charType_modelName, premission_read, 0),
    serialNumber(index+4, charType_serialNumber, premission_read, 0),
    identify(index+5) {
        name.setValue(deviceName);
        manufactuer.setValue(manufactuerName);
        modelName.setValue(deviceName);
        serialNumber.setValue(deviceUUID);
    }
    virtual short numberOfCharacteristics() { return 5; }
    virtual characteristics *characteristicsAtIndex(int index) {
        switch (index-1-serviceID) {
            case 0:
                return &name;
            case 1:
                return &manufactuer;
            case 2:
                return &modelName;
            case 3:
                return &serialNumber;
            case 4:
                return &identify;
        }
        return 0;
    }
};
void handleAccessory(const char *request, unsigned int requestLen, char **reply, unsigned int *replyLen);
