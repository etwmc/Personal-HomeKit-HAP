//
//  PHKAccessory.c
//  Workbench
//
//  Created by Wai Man Chan on 9/27/14.
//
//

#include "PHKAccessory.h"
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

const char hapJsonType[] = "application/hap+json";
const char pairingTlv8Type[] = "application/pairing+tlv8";

bool lightOn = false;

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

//Wrap to JSON
inline string wrap(const char *str) { return (string)"\""+str+"\""; }
inline string attribute(unsigned short type, unsigned short acclaim, int p, bool value) {
    string result;
    
    result += wrap("value")+":";
    if (value) result += "true";
    else result += "false";
    result += ",";
    
    result += wrap("perms")+":";
    result += "[";
    if (p & premission_read) result += wrap("pr")+",";
    if (p & premission_write) result += wrap("pw")+",";
    if (p & premission_update) result += wrap("ev")+",";
    result = result.substr(0, result.size()-1);
    result += "]";
    result += ",";
    
    char tempStr[4];
    snprintf(tempStr, 4, "%X", type);
    result += wrap("type")+":"+wrap(tempStr);
    result += ",";
    
    snprintf(tempStr, 4, "%hd", acclaim);
    result += wrap("iid")+":"+tempStr;
    result += ",";
    
    result += "\"format\":\"bool\"";
    
    return "{"+result+"}";
}
inline string attribute(unsigned short type, unsigned short acclaim, int p, int value, int minVal, int maxVal, int step, unit valueUnit) {
    string result;
    char tempStr[4];
    
    snprintf(tempStr, 4, "%d", value);
    
    result += wrap("value")+":"+tempStr;
    result += ",";
    
    snprintf(tempStr, 4, "%d", minVal);
    if (minVal != INT32_MIN)
        result += wrap("minValue")+":"+tempStr+",";
    
    snprintf(tempStr, 4, "%d", maxVal);
    if (maxVal != INT32_MAX)
        result += wrap("maxValue")+":"+tempStr+",";
    
    snprintf(tempStr, 4, "%d", step);
    if (step > 0)
        result += wrap("minStep")+":"+tempStr+",";
    
    result += wrap("perms")+":";
    result += "[";
    if (p & premission_read) result += wrap("pr")+",";
    if (p & premission_write) result += wrap("pw")+",";
    if (p & premission_update) result += wrap("ev")+",";
    result = result.substr(0, result.size()-1);
    result += "]";
    result += ",";
    
    snprintf(tempStr, 4, "%X", type);
    result += wrap("type")+":"+wrap(tempStr);
    result += ",";
    
    snprintf(tempStr, 4, "%hd", acclaim);
    result += wrap("iid")+":"+tempStr;
    result += ",";
    
    switch (valueUnit) {
        case unit_arcDegree:
            result += wrap("unit")+":"+wrap("arcdegrees")+",";
            break;
        case unit_celsius:
            result += wrap("unit")+":"+wrap("celsius")+",";
            break;
        case unit_percentage:
            result += wrap("unit")+":"+wrap("percentage")+",";
            break;
    }
    
    result += "\"format\":\"int\"";
    
    return "{"+result+"}";
}
inline string attribute(unsigned short type, unsigned short acclaim, int p, float value, float minVal, float maxVal, float step, unit valueUnit) {
    string result;
    char tempStr[4];
    
    snprintf(tempStr, 4, "%f", value);
    
    result += wrap("value")+":"+tempStr;
    result += ",";
    
    snprintf(tempStr, 4, "%f", minVal);
    if (minVal != INT32_MIN)
        result += wrap("minValue")+":"+tempStr+",";
    
    snprintf(tempStr, 4, "%f", maxVal);
    if (maxVal != INT32_MAX)
        result += wrap("maxValue")+":"+tempStr+",";
    
    snprintf(tempStr, 4, "%f", step);
    if (step > 0)
        result += wrap("minStep")+":"+tempStr+",";
    
    result += wrap("perms")+":";
    result += "[";
    if (p & premission_read) result += wrap("pr")+",";
    if (p & premission_write) result += wrap("pw")+",";
    if (p & premission_update) result += wrap("ev")+",";
    result = result.substr(0, result.size()-1);
    result += "]";
    result += ",";
    
    snprintf(tempStr, 4, "%X", type);
    result += wrap("type")+":"+wrap(tempStr);
    result += ",";
    
    snprintf(tempStr, 4, "%hd", acclaim);
    result += wrap("iid")+":"+tempStr;
    result += ",";
    
    switch (valueUnit) {
        case unit_arcDegree:
            result += wrap("unit")+":"+wrap("arcdegrees")+",";
            break;
        case unit_celsius:
            result += wrap("unit")+":"+wrap("celsius")+",";
            break;
        case unit_percentage:
            result += wrap("unit")+":"+wrap("percentage")+",";
            break;
    }
    
    result += "\"format\":\"float\"";
    
    return "{"+result+"}";
}
inline string attribute(unsigned short type, unsigned short acclaim, int p, string value, unsigned short len) {
    string result;
    char tempStr[4];
    
    result += wrap("value")+":"+wrap(value.c_str());
    result += ",";
    
    result += wrap("perms")+":";
    result += "[";
    if (p & premission_read) result += wrap("pr")+",";
    if (p & premission_write) result += wrap("pw")+",";
    if (p & premission_update) result += wrap("ev")+",";
    result = result.substr(0, result.size()-1);
    result += "]";
    result += ",";
    
    snprintf(tempStr, 4, "%X", type);
    result += wrap("type")+":"+wrap(tempStr);
    result += ",";
    
    snprintf(tempStr, 4, "%hd", acclaim);
    result += wrap("iid")+":"+tempStr;
    result += ",";
    
    if (len > 0) {
        snprintf(tempStr, 4, "%hd", len);
        result += wrap("maxLen")+":"+tempStr;
        result += ",";
    }
    
    result += "\"format\":\"string\"";
    
    return "{"+result+"}";
}
inline string arrayWrap(string *s, unsigned short len) {
    string result;
    
    result += "[";
    
    for (int i = 0; i < len; i++) {
        result += s[i]+",";
    }
    result = result.substr(0, result.size()-1);
    
    result += "]";
    
    return result;
}
inline string dictionaryWrap(string *key, string *value, unsigned short len) {
    string result;
    
    result += "{";
    
    for (int i = 0; i < len; i++) {
        result += wrap(key[i].c_str())+":"+value[i]+",";
    }
    result = result.substr(0, result.size()-1);
    
    result += "}";
    
    return result;
}

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
    virtual string describe() {
        return attribute(type, iid, premission, _value);
    }
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
    virtual string describe() {
        return attribute(type, iid, premission, _value, _minVal, _maxVal, _step, _unit);
    }
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
    virtual string describe() {
        return attribute(type, iid, premission, _value, _minVal, _maxVal, _step, _unit);
    }
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
    virtual string describe() {
        return attribute(type, iid, premission, _value, maxLen);
    }
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
    virtual void identify() {}
    virtual string describe() {
        string a = boolCharacteristics::describe();
        return a;
    }
};

//Abstract Layer of object
class Service {
public:
    const int serviceID, uuid;
    Service(int _serviceID, int _uuid): serviceID(_serviceID), uuid(_uuid) {}
    inline virtual short numberOfCharacteristics() { return 0; }
    inline virtual characteristics *characteristicsAtIndex(int index) { return 0; }
    string describe() {
        string keys[3] = {"iid", "type", "characteristics"};
        string values[3];
        {
            char temp[8];
            snprintf(temp, 8, "%d", serviceID);
            values[0] = temp;
        }
        {
            char temp[8];
            snprintf(temp, 8, "\"%X\"", uuid);
            values[1] = temp;
        }
        {
            int no = numberOfCharacteristics();
            string *chars = new string[no];
            for (int i = 0; i < no; i++) {
                chars[i] = characteristicsAtIndex(i+1+serviceID)->describe();
            }
            values[2] = arrayWrap(chars, no);
            delete [] chars;
        }
        return dictionaryWrap(keys, values, 3);
    }
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
    string describe() {
        string keys[2];
        string values[2];
        
        {
            keys[0] = "aid";
            char temp[8];
            sprintf(temp, "%d", aid);
            values[0] = temp;
        }
                   
        {
            //Form services list
            int noOfService = numberOfService();
            string *services = new string[noOfService];
            for (int i = 0; i < noOfService; i++) {
                services[i] = serviceAtIndex(i+1)->describe();
            }
            keys[1] = "services";
            values[1] = arrayWrap(services, noOfService);
            delete [] services;
        }
        
        string result = dictionaryWrap(keys, values, 2);
        return result;
    }
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
    string describe() {
        int numberOfAcc = numberOfAccessory();
        string *desc = new string[numberOfAcc];
        for (int i = 0; i < numberOfAcc; i++) {
            desc[i] = accessoryAtIndex(i)->describe();
        }
        string result = arrayWrap(desc, numberOfAcc);
        delete [] desc;
        string key = "accessories";
        result = dictionaryWrap(&key, &result, 1);
        return result;
    }
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
    inline virtual short numberOfCharacteristics() { return 5; }
    inline virtual characteristics *characteristicsAtIndex(int index) {
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

class lightPowerState: public boolCharacteristics {
public:
    lightPowerState(int index): boolCharacteristics(index, charType_on, premission_read|premission_write){}
    void setValue(string str) {
        this->boolCharacteristics::setValue(str);
        if (_value) {
            setLightStrength(255);
        } else {
            setLightStrength(0);
        }
    }
};

class lightBrightness: public intCharacteristics {
public:
    lightBrightness(int index):intCharacteristics(index, charType_brightness, premission_read|premission_write, 0, 100, 1, unit_percentage) {}
    void setValue(string str) {
        this->intCharacteristics::setValue(str);
        setLightStrength(2.55*_value);
    }
};

class lightService: public Service {
    stringCharacteristics serviceName;
    lightPowerState powerState;
    lightBrightness brightness;
public:
    lightService(int index): Service(index, charType_lightBulb),
    serviceName(index+1, charType_serviceName, premission_read, 0), powerState(index+2), brightness(index+3)
    {
        serviceName.setValue(deviceName);
        powerState.setValue("false");
    }
    inline virtual short numberOfCharacteristics() { return 3; }
    inline virtual characteristics *characteristicsAtIndex(int index) {
        switch (index-1-serviceID) {
            case 0:
                return &serviceName;
            case 1:
                return &powerState;
            case 2:
                return &brightness;
        }
        return 0;
    }
};



//For bridge, create more than one subclass, and insert in main accessory
//Also change the MainAccessorySet
class MainAccessory: public Accessory {
    infoService info;
    lightService light;
public:
    MainAccessory(int aid): Accessory(aid),
    info(1), light(info.serviceID+info.numberOfCharacteristics()+1) {}
    inline virtual short numberOfService() { return 2; }
    inline virtual Service *serviceAtIndex(int index) {
        switch (index-1) {
            case 0:
                return &info;
            case 1:
                return &light;
        }
        return 0;
    }
};

//For bridge, change the subject to dynamic assign
class MainAccessorySet: public AccessorySet {
    MainAccessory acc;
public:
    MainAccessorySet(): acc(1) {
    }
    short numberOfAccessory() { return 1; }
    Accessory * accessoryAtIndex(int index) { return &acc; }
};

MainAccessorySet accSet;

void announce() {
    
    string desc = accSet.describe();
    
    char *reply = new char[1024];
    int len = snprintf(reply, 1024, "EVENT/1.1 200 OK\r\n\
                       Content-Type: application/hap+json\r\n\
                       Content-Length: %lu\r\n\r\n%s", desc.length(), desc.c_str());
    broadcastMessage(reply, len);
    delete [] reply;
}

void handleAccessory(const char *request, unsigned int requestLen, char **reply, unsigned int *replyLen) {
#if HomeKitLog == 1
    printf("Receive request\n");
#endif
    int index = 5;
    char method[5];
    {
        //Read method
        method[4] = 0;
        bcopy(request, method, 4);
        if (method[3] == ' ') {
            method[3] = 0;
            index = 4;
        }
    }
    
    char path[32];
    int i;
    for (i = 0; i < 32 && request[index] != ' '; i++, index++) {
        path[i] = request[index];
    }
    path[i] = 0;
    
    const char *dataPtr = request;
    while (true) {
        dataPtr = &dataPtr[1];
        if (dataPtr[0] == '\r' && dataPtr[1] == '\n' && dataPtr[2] == '\r' && dataPtr[3] == '\n') break;
    }
    
    dataPtr += 4;
    
    char *replyData = NULL;  unsigned short replyDataLen = 0;
    
    int statusCode;
    
    const char *protocol = "HTTP/1.1";
    const char *returnType = hapJsonType;
    
    if (strcmp(path, "/accessories") == 0) {
        //Publish the characterists of the accessories
#if HomeKitLog == 1
        printf("Ask for accessories info\n");
#endif
        statusCode = 200;
        string desc = accSet.describe();
        replyDataLen = desc.length();
        replyData = new char[replyDataLen+1];
        bcopy(desc.c_str(), replyData, replyDataLen);
        replyData[replyDataLen] = 0;
    } else if (strcmp(path, "/pairings") == 0) {
        //Pairing with new user
#if HomeKitLog == 1
        printf("Add new user\n");
#endif
        PHKNetworkMessage msg(request);
        statusCode = 200;
        if (*msg.data.dataPtrForIndex(0) == 3) {
            PHKKeyRecord controllerRec;
            bcopy(msg.data.dataPtrForIndex(3), controllerRec.publicKey, 32);
            bcopy(msg.data.dataPtrForIndex(1), controllerRec.controllerID, 36);
            addControllerKey(controllerRec);
            PHKNetworkMessageDataRecord drec;
            drec.activate = true; drec.data = new char[1]; *drec.data = 2;
            drec.index = 6; drec.length = 1;
            PHKNetworkMessageData data;
            data.addRecord(drec);
            data.rawData((const char **)&replyData, &replyDataLen);
            returnType = pairingTlv8Type;
            statusCode = 200;
        } else {
#if HomeKitLog == 1
            printf("Delete user");
#endif
            PHKKeyRecord controllerRec;
            bcopy(msg.data.dataPtrForIndex(1), controllerRec.controllerID, 36);
            removeControllerKey(controllerRec);
            PHKNetworkMessageDataRecord drec;
            drec.activate = true; drec.data = new char[1]; *drec.data = 2;
            drec.index = 6; drec.length = 1;
            PHKNetworkMessageData data;
            data.addRecord(drec);
            data.rawData((const char **)&replyData, &replyDataLen);
            returnType = pairingTlv8Type;
            statusCode = 200;
        }
    } else if (strncmp(path, "/characteristics", 16) == 0){
        if (strncmp(method, "GET", 3) == 0) {
            //Read characteristics
            int aid = 0;    int iid = 0;
            sscanf(path, "/characteristics?id=%d.%d", &aid, &iid);
            Accessory *a = accSet.accessoryAtIndex(aid);
            if (a != NULL) {
                characteristics *c = a->characteristicsAtIndex(iid);
                if (c != NULL) {
#if HomeKitLog == 1
                    printf("Ask for one characteristics: %d . %d\n", aid, iid);
#endif
                    char c1[3], c2[3];
                    sprintf(c1, "%d", aid);
                    sprintf(c2, "%d", iid);
                    string s[3] = {string(c1), string(c2), c->value()};
                    string k[3] = {"aid", "iid", "value"};
                    string result = dictionaryWrap(k, s, 3);
                    string d = "characteristics";
                    result = arrayWrap(&result, 1);
                    result = dictionaryWrap(&d, &result, 1);
                    
                    replyDataLen = result.length();
                    replyData = new char[replyDataLen+1];
                    replyData[replyDataLen] = 0;
                    bcopy(result.c_str(), replyData, replyDataLen);
                    statusCode = 200;
                } else {
                    statusCode = 404;
                }

            } else {
                statusCode = 404;
            }
        } else if (strncmp(method, "PUT", 3) == 0) {
            //Change characteristics
            int aid = 0;    int iid = 0; char value[16];
            sscanf(dataPtr, "{\"characteristics\":[{\"aid\":%d,\"iid\":%d,\"value\":%s}]}", &aid, &iid, value);
#if HomeKitLog == 1
            printf("Ask to change one characteristics: %d . %d -> %s\n", aid, iid, value);
#endif
            Accessory *a = accSet.accessoryAtIndex(aid);
            if (a==NULL) {
                statusCode = 400;
            } else {
                characteristics *c = a->characteristicsAtIndex(iid);
                if (c==NULL) {
                } else {
                    if (c->writable()) {
                        c->setValue(value);
                        
                        statusCode = 204;
                        if (c->update());
                            //Broadcast change to everyone
                            //announce();
                    } else {
                        statusCode = 400;
                    }
                }
                
            }
            
            
            
        } else {
            return;
        }
    } else {
        //Error
#if HomeKitLog == 1
        printf("Ask for something I don't know\n");
#endif
        printf("%s\n", request);
        printf("%s", path);
        statusCode = 404;
    }
    
    *reply = new char[1536];
    bzero(*reply, 1536);
    int len = snprintf(*reply, 1536, "%s %d OK\r\n\
Content-Type: %s\r\n\
Content-Length: %u\r\n\r\n", protocol, statusCode, returnType, replyDataLen);
    
    (*replyLen) = len+replyDataLen;
    
    if (replyData) {
        bcopy(replyData, &(*reply)[len], replyDataLen+1);
        delete [] replyData;
    }
    
#if HomeKitLog == 1 && HomeKitReplyHeaderLog==1
    printf("Reply: %s\n", *reply);
#endif
    
}
